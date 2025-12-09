#include "Game.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/OperationCenter.h"
#include "data/SoundCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include "Player.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <vector>
#include <cstring>
#include <optional>
#include "scene/Menu.h"
#include "scene/Farm.h"
#include "scene/LevelMenu.h"
#include "scene/Store.h"
#include "scene/Profile.h"

// fixed settings
constexpr char game_icon_img_path[] = "./assets/image/game_icon.png";
constexpr char game_start_sound_path[] = "./assets/sound/growl.wav";
constexpr char background_img_path[] = "./assets/image/StartBackground.jpg";
constexpr char background_sound_path[] = "./assets/sound/BackgroundMusic.ogg";

/**
 * @brief Game entry.
 * @details The function processes all allegro events and update the event state to a generic data storage (i.e. DataCenter).
 * For timer event, the game_update and game_draw function will be called if and only if the current is timer.
 */
void
Game::execute() {
	DataCenter *DC = DataCenter::get_instance();
	// main game loop
	bool run = true;
	while(run) {
		// process all events here
		al_wait_for_event(event_queue, &event);
		switch(event.type) {
			case ALLEGRO_EVENT_TIMER: {
				run &= game_update();
				game_draw();
				break;
			} case ALLEGRO_EVENT_DISPLAY_CLOSE: { // stop game
				run = false;
				break;
			} case ALLEGRO_EVENT_KEY_DOWN: {
				DC->key_state[event.keyboard.keycode] = true;
				break;
			} case ALLEGRO_EVENT_KEY_UP: {
				DC->key_state[event.keyboard.keycode] = false;
				break;
			} case ALLEGRO_EVENT_MOUSE_AXES: {
				DC->mouse.x = event.mouse.x;
				DC->mouse.y = event.mouse.y;
				break;
			} case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
				DC->mouse_state[event.mouse.button] = true;
				break;
			} case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
				DC->mouse_state[event.mouse.button] = false;
				break;
			} default: break;
		}
	}
}

/**
 * @brief Initialize all allegro addons and the game body.
 * @details Only one timer is created since a game and all its data should be processed synchronously.
 */
Game::Game(bool testMode) {
	DataCenter *DC = DataCenter::get_instance();
	GAME_ASSERT(al_init(), "failed to initialize allegro.");

	// initialize allegro addons
	bool addon_init = true;
	addon_init &= al_init_primitives_addon();
	addon_init &= al_init_font_addon();
	addon_init &= al_init_ttf_addon();
	addon_init &= al_init_image_addon();
	addon_init &= al_init_acodec_addon();
	GAME_ASSERT(addon_init, "failed to initialize allegro addons.");

	if(testMode) {
		timer = nullptr;
		event_queue = nullptr;
		display = nullptr;
		debug_log("Game initialized in test mode.\n");
		return;
	}

	// initialize events
	bool event_init = true;
	event_init &= al_install_keyboard();
	event_init &= al_install_mouse();
	event_init &= al_install_audio();
	GAME_ASSERT(event_init, "failed to initialize allegro events.");

	// initialize game body
	GAME_ASSERT(
		timer = al_create_timer(1.0 / DC->FPS),
		"failed to create timer.");
	GAME_ASSERT(
		event_queue = al_create_event_queue(),
		"failed to create event queue.");
	GAME_ASSERT(
		display = al_create_display(DC->window_width, DC->window_height),
		"failed to create display.");

	debug_log("Game initialized.\n");
	game_init();
}

/**
 * @brief Initialize all auxiliary resources.
 */
void
Game::game_init() {
	DataCenter *DC = DataCenter::get_instance();
	SoundCenter *SC = SoundCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();
	// set window icon
	game_icon = IC->get(game_icon_img_path);
	al_set_display_icon(display, game_icon);

	// register events to event_queue
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

	// init sound setting
	SC->init();

	// init font setting
	FC->init();

	//player init
	DC->player->load();
	
	//scene init
	Menu::get()->init();
	LevelMenu::getInstance()->init();

	// game start
	background = IC->get(background_img_path);
	debug_log("Game state: change to START\n");
	state = STATE::START;
	al_start_timer(timer);
}

/**
 * @brief The function processes all data update.
 * @details The behavior of the whole game body is determined by its state.
 * @return Whether the game should keep running (true) or reaches the termination criteria (false).
 * @see Game::STATE
 */
bool
Game::game_update() {
	DataCenter *DC = DataCenter::get_instance();
	OperationCenter *OC = OperationCenter::get_instance();
	SoundCenter *SC = SoundCenter::get_instance();
	static ALLEGRO_SAMPLE_INSTANCE *background = nullptr;

	switch(state) {
		case STATE::START: {
			static bool is_played = false;
			static ALLEGRO_SAMPLE_INSTANCE *instance = nullptr;
			if(!is_played) {
				instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
				is_played = true;
			}

			// if(!SC->is_playing(instance)) {
			// 	debug_log("<Game> state: change to MENU from START\n");
			// 	state = STATE::MENU;
			// }

			if(DC->key_state[ALLEGRO_KEY_ENTER] && !DC->prev_key_state[ALLEGRO_KEY_ENTER]){
				debug_log("<Game> state: change to MENU from START\n");
				state = STATE::MENU;
			}

			break;
		} 
		
		case STATE::MENU: {
			static bool BGM_played = false;
			if(!BGM_played) {
				background = SC->play(background_sound_path, ALLEGRO_PLAYMODE_LOOP);
				BGM_played = true;
			}

			// debug_log("<Game> state: MENU\n");

			auto MS = Menu::get();
			MS->update();

			STATE req = Player::getPlayer()->getRequest();
			if(req!=state){
				scene_init(req);
				debug_log("<Game> state: toggle from MENU\n");
				state = req;
			}

			break;
		}
		case STATE::FARM:{
			// debug_log("<Game> state: FARM\n");

			auto FS = Farm::get();
			FS->update();

			STATE req = Player::getPlayer()->getRequest();
			if(req!=state){
				scene_init(req);
				debug_log("<Game> state: toggle from FARM\n");
				state = req;
			}
			break;
		}
		case STATE::PROFILE:{
			auto PS = Profile::get();
			PS->update();

			debug_log("<Game> state: PROFILE\n");
			STATE req = Player::getPlayer()->getRequest();
			if(req!=state){
				scene_init(req);
				debug_log("<Game> state: toggle from PROFILE\n");
				state = req;
			}
			break;
		}
		case STATE::STORE: {
			auto SS = Store::get();
			SS->update();

			debug_log("<Game> state: STORE\n");
			STATE req = Player::getPlayer()->getRequest();
			if(req!=state){
				scene_init(req);
				debug_log("<Game> state: toggle from STORE\n");
				state = req;
			}
			break;
		} 
		case STATE::PAUSE: {
			if(DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
				SC->toggle_playing(background);
				debug_log("<Game> state: change to MENU from PAUSE\n");
				state = STATE::MENU;
			}
			break;
		} 
		case STATE::LEVEL: {
			LevelMenu::getInstance()->update();
			STATE req = static_cast<STATE>(Player::getPlayer()->getRequest());
			if(req != state){
				state = req;
			}
			break;
		}
		case STATE::END: {
			return false;
		}
	}
	// If the game is not paused, we should progress update.
	if(state != STATE::PAUSE) {
		DC->player->update();
		SC->update();
		
		if(state != STATE::START) {
			OC->update();
		}
	}
	// game_update is finished. The states of current frame will be previous states of the next frame.
	memcpy(DC->prev_key_state, DC->key_state, sizeof(DC->key_state));
	memcpy(DC->prev_mouse_state, DC->mouse_state, sizeof(DC->mouse_state));

	//debug mouse state
	// debug_log("%d, %d, %d, %d\n", DC->mouse_state[0], DC->mouse_state[1], DC->mouse_state[2], DC->mouse_state[3]);
	// debug_log("%d, %d\n", DC->mouse_state[1], DC->prev_mouse_state[1]);
	
	return true;
}

/**
 * @brief Draw the whole game and objects.
 */
void
Game::game_draw() {
	DataCenter *DC = DataCenter::get_instance();
	OperationCenter *OC = OperationCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();
	
	// Flush the screen first.
	al_clear_to_color(al_map_rgb(100, 100, 100));

	switch(state) {
		case STATE::START: {
			auto start_menu = ImageCenter::get_instance()->get("./assets/image/scene/start.png");
			al_draw_bitmap(start_menu, 0, 0, 0);

			break;
		} 
		case STATE::MENU: {
			auto MS = Menu::get();
			MS->draw();
			break;
		}
		case STATE::FARM: {
			auto FS = Farm::get();
			FS->draw();
			break;
		} 
		case STATE::STORE: {
			auto SS = Store::get();
			SS->draw();
			break;
		}
		case STATE::PROFILE:{
			auto PS = Profile::get();
			PS->draw();
			break;
		}
		case STATE::LEVEL: {
			LevelMenu::getInstance()->draw();
			break;
		}
		case STATE::PAUSE: {
			// game layout cover
			al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(50, 50, 50, 64));
			al_draw_text(
				FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
				DC->window_width/2., DC->window_height/2.,
				ALLEGRO_ALIGN_CENTRE, "GAME PAUSED");
			break;
		} 
		case STATE::END: {
		}
	}
	al_flip_display();
}

void Game::scene_init(STATE st){
	switch (st){
	case STATE::MENU:{
		Menu::get()->init();
		break;
	}
	case STATE::FARM:{
		Farm::get()->init();
		break;
	}
	case STATE::LEVEL: {
		LevelMenu::getInstance()->scene_init();
		break;
	}
	case STATE::STORE:{
		Store::get()->init();
		break;
	}
	case STATE::PROFILE: {
		Profile::get()->init();
		break;
	}
	default:
		break;
	}
}

Game::~Game() {
	if(display) al_destroy_display(display);
	if(timer) al_destroy_timer(timer);
	if(event_queue) al_destroy_event_queue(event_queue);
}
