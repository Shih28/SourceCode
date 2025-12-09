#include <iostream>
#include <string>
#include <allegro5/allegro.h>
#include "../shapes/Point.h"
#include "../shapes/Circle.h"
#include "../shapes/Rectangle.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../data/SoundCenter.h"
#include "../Game.h"
#include "../Utils.h"
#include "../Player.h"
#include "LevelMenu.h"
#include "LevelButton.h"

namespace levelmenu {
  constexpr char background_image_path[] = "./assets/image/levelmenu/background.png";
  constexpr char button_image_path[] = "./assets/image/levelmenu/button.png";
  constexpr char button_hover_image_path[] = "./assets/image/levelmenu/button_hover.png";
  constexpr char button_selected_image_path[] = "./assets/image/levelmenu/button_selected.png";
  constexpr char level_intro_image_path[] = "./assets/image/levelmenu/level_intro.png";
  constexpr char return_button_image_path[] = "./assets/image/levelmenu/return.png";
  constexpr char return_hover_button_image_path[] = "./assets/image/levelmenu/return_hover.png";
  constexpr char button_selected_sound_path[] = "./assets/sound/levelmenu/click.mp3";
  constexpr char button_hover_sound_path[] = "./assets/sound/levelmenu/hover.mp3";
  constexpr char level_passed_file[] = "./assets/config/levelmenu/levels_passed.txt";
}

void LevelMenu::init()
{
  ALLEGRO_FILE *file = al_fopen(levelmenu::level_passed_file, "r");
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
  return_button_image = IC->get(levelmenu::return_button_image_path);
  return_hover_button_image = IC->get(levelmenu::return_hover_button_image_path);

  level_buttons[0] = LevelButton(225, 170, 1);
  level_buttons[1] = LevelButton(215, 490, 2);
  level_buttons[2] = LevelButton(570, 305, 3);
  level_buttons[3] = LevelButton(660, 80, 4);
  level_buttons[4] = LevelButton(915, 160, 5);
  level_buttons[5] = LevelButton(890, 520, 6);
  
  return_button_rect = Rectangle(30, 525, 30 + al_get_bitmap_width(return_button_image), 525 + al_get_bitmap_height(return_button_image));
}

void LevelMenu::scene_init() {
  selected_level_button = NULL;
}

void LevelMenu::update()
{
  DataCenter *DC = DataCenter::get_instance();
  SoundCenter *SC = SoundCenter::get_instance();

  if (return_button_rect.overlap(DC->mouse)) {
    if (DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
      SC->play(levelmenu::button_selected_sound_path, ALLEGRO_PLAYMODE_ONCE);
      Player::getPlayer()->setrequest(static_cast<int>(Game::STATE::MENU));
    } 
  }

  for (LevelButton& button : level_buttons) {
    if (Circle(button.position + Point(60, 60), 50).overlap(DC->mouse)) {
      if (!button.hover) {
        button.hover = true;
        SC->play(levelmenu::button_hover_sound_path, ALLEGRO_PLAYMODE_ONCE);
      }
      if (DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
        selected_level_button = &button;
        SC->play(levelmenu::button_selected_sound_path, ALLEGRO_PLAYMODE_ONCE);
        debug_log("<LevelMenu> selected level %d\n", button.level);
      }
    } else {
      button.hover = false;
    }
  }
}

void LevelMenu::draw()
{
  DataCenter *DC = DataCenter::get_instance();

  al_draw_bitmap(background, 0, 0, 0);
  
  if (return_button_rect.overlap(DC->mouse)) {
    al_draw_bitmap(return_hover_button_image, return_button_rect.x1, return_button_rect.y1, 0);
  } else {
    al_draw_bitmap(return_button_image, return_button_rect.x1, return_button_rect.y1, 0);
  }

  for (LevelButton& button : level_buttons) {
    if (&button == selected_level_button) {
      al_draw_bitmap(button_hover_image, button.position.x, button.position.y, 0);
    } else if (Circle(button.position + Point(60, 60), 50).overlap(DC->mouse)) {
      al_draw_bitmap(button_selected_image, button.position.x, button.position.y, 0);
    } else {
      al_draw_bitmap(button_image, button.position.x, button.position.y, 0);
    }
  }
}

void LevelMenu::end() {

}