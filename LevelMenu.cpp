#include <iostream>
#include <optional>
#include <string>
#include <allegro5/allegro.h>
#include "shapes/Point.h"
#include "shapes/Circle.h"
#include "data/DataCenter.h"
#include "data/SoundCenter.h"
#include "LevelMenu.h"
#include "LevelButton.h"
#include "Utils.h"

namespace levelmenu {
  constexpr char background_image_path[] = "./assets/image/levelmenu/background.png";
  constexpr char button_image_path[] = "./assets/image/levelmenu/button.png";
  constexpr char button_hover_image_path[] = "./assets/image/levelmenu/button_hover.png";
  constexpr char button_selected_image_path[] = "./assets/image/levelmenu/button_selected.png";
  constexpr char level_intro_image_path[] = "./assets/image/levelmenu/level_intro.png";
  constexpr char button_selected_sound_path[] = "./assets/sound/levelmenu/click.mp3";
  constexpr char button_hover_sound_path[] = "./assets/sound/levelmenu/hover.mp3";
}

void LevelMenu::init()
{
  ALLEGRO_FILE *file = al_fopen("./assets/config/levelmenu/levels_passed.txt", "r");
  GAME_ASSERT(file != nullptr, "<LevelMenu> cannot find level_passed setting from assets");

  char buffer[5];
  size_t bytes_read = al_fread(file, buffer, sizeof(buffer) - 1);
  buffer[bytes_read] = '\0';

  try {
    levels_passed = std::stoi(buffer);
  } catch(std::invalid_argument& e) {
    debug_log("<LevelMenu> Warning: invalid levels_passed setting from assets");
    levels_passed = 1;
  }
  GAME_ASSERT(1 <= levels_passed && levels_passed <= 10, "<LevelMenu> invalid levels_passed settomg from assets");

  al_fclose(file);
  
  ImageCenter *IC = ImageCenter::get_instance();
  background = IC->get(levelmenu::background_image_path);
  button_image = IC->get(levelmenu::button_image_path);
  button_hover_image = IC->get(levelmenu::button_hover_image_path);
  button_selected_image = IC->get(levelmenu::button_selected_image_path);
  level_intro_image = IC->get(levelmenu::level_intro_image_path);

  level_buttons[0] = LevelButton(225, 170, 1);
  level_buttons[1] = LevelButton(215, 490, 2);
  level_buttons[2] = LevelButton(570, 305, 3);
  level_buttons[3] = LevelButton(660, 80, 4);
  level_buttons[4] = LevelButton(915, 160, 5);
  level_buttons[5] = LevelButton(890, 520, 6);

  last_mouse_state = false;
}

void LevelMenu::update()
{

}

void LevelMenu::draw()
{
  DataCenter *DC = DataCenter::get_instance();
  SoundCenter *SC = SoundCenter::get_instance();

  al_draw_bitmap(background, 0, 0, 0);

  for (LevelButton& button : level_buttons) {
    if (&button == selected_level_button) {
      al_draw_bitmap(button_hover_image, button.position.x, button.position.y, 0);
    } else if (Circle(button.position + Point(60, 60), 50).overlap(DC->mouse)) {
      if (!button.hover) {
        button.hover = true;
        SC->play(levelmenu::button_hover_sound_path, ALLEGRO_PLAYMODE_ONCE);
      }
      if (DC->mouse_state[1] && !last_mouse_state) {
        selected_level_button = &button;
        SC->play(levelmenu::button_selected_sound_path, ALLEGRO_PLAYMODE_ONCE);
        debug_log("<LevelMenu> selected level %d\n", button.level);
      }
      al_draw_bitmap(button_selected_image, button.position.x, button.position.y, 0);
    } else {
      button.hover = false;
      al_draw_bitmap(button_image, button.position.x, button.position.y, 0);
    }
  }

  last_mouse_state = DC->mouse_state[1];
}

std::optional<Game::STATE> LevelMenu::get_game_state_update_request()
{
  return next_state;
}
