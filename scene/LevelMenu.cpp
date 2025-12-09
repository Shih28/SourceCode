#include <iostream>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "../shapes/Point.h"
#include "../shapes/Circle.h"
#include "../shapes/Rectangle.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../data/SoundCenter.h"
#include "../data/FontCenter.h"
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
  constexpr char close_button_image_path[] = "./assets/image/levelmenu/close.png";
  constexpr char close_hover_button_image_path[] = "./assets/image/levelmenu/close_hover.png";
  constexpr char go_button_image_path[] = "./assets/image/levelmenu/go.png";
  constexpr char go_hover_button_image_path[] = "./assets/image/levelmenu/go_hover.png";
  constexpr char button_selected_sound_path[] = "./assets/sound/levelmenu/click.mp3";
  constexpr char button_hover_sound_path[] = "./assets/sound/levelmenu/hover.mp3";
  constexpr char level_passed_file[] = "./assets/config/levelmenu/levels_passed.txt";
  
  const char* level_intro_texts[][3] = {
    {"Level 1: Welcome to the Adventure!", "Defeat basic enemies.", ""},
    {"Level 2: Rising Challenge", "Face stronger foes.", ""},
    {"Level 3: The Trial Begins", "Test your strategy.", ""},
    {"Level 4: Dark Forest", "Survive the shadows.", ""},
    {"Level 5: Mountain Peak", "Conquer the heights.", ""},
    {"Level 6: Final Battle", "Defeat the ultimate boss!", ""}
  };
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
  close_button_image = IC->get(levelmenu::close_button_image_path);
  close_hover_button_image = IC->get(levelmenu::close_hover_button_image_path);
  go_button_image = IC->get(levelmenu::go_button_image_path);
  go_hover_button_image = IC->get(levelmenu::go_hover_button_image_path);

  level_buttons[0] = LevelButton(225, 170, 1);
  level_buttons[1] = LevelButton(215, 490, 2);
  level_buttons[2] = LevelButton(570, 305, 3);
  level_buttons[3] = LevelButton(660, 80, 4);
  level_buttons[4] = LevelButton(915, 160, 5);
  level_buttons[5] = LevelButton(890, 520, 6);
  
  return_button_rect = Rectangle(30, 525, 30 + al_get_bitmap_width(return_button_image), 525 + al_get_bitmap_height(return_button_image));
  
  // Position close button at top-right of intro image and go button at bottom-right
  int intro_x = 500;
  int intro_y = 100;
  int intro_width = al_get_bitmap_width(level_intro_image);
  int intro_height = al_get_bitmap_height(level_intro_image);
  
  close_button_rect = Rectangle(intro_x + 336, intro_y + 29, 
                                 intro_x + 336 + al_get_bitmap_width(close_button_image), 
                                 intro_y + 29 + al_get_bitmap_height(close_button_image));
  
  go_button_rect = Rectangle(intro_x + 7, intro_y + 380,
                              intro_x + 7 + al_get_bitmap_width(go_button_image),
                              intro_y + 380 + al_get_bitmap_height(go_button_image));

  scene_init();
}

void LevelMenu::scene_init() {
  selected_level_button = NULL;
  intro_animating = false;
  intro_current_y = -500.0;
  intro_velocity = 0.0;
  intro_target_y = 110;
  animation_frame = 0;
}

void LevelMenu::update()
{
  DataCenter *DC = DataCenter::get_instance();
  SoundCenter *SC = SoundCenter::get_instance();

  if (return_button_rect.overlap(DC->mouse)) {
    if (DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
      SC->play(levelmenu::button_selected_sound_path, ALLEGRO_PLAYMODE_ONCE);
      Player::getPlayer()->setrequest(Game::STATE::MENU);
    } 
  }

  // Handle close button (closes level intro)
  if (selected_level_button != NULL && close_button_rect.overlap(DC->mouse)) {
    if (DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
      selected_level_button = NULL;
      intro_animating = false;
      SC->play(levelmenu::button_selected_sound_path, ALLEGRO_PLAYMODE_ONCE);
      debug_log("<LevelMenu> Close button clicked\n");
    }
  }

  // Update intro animation
  if (intro_animating) {
    const double gravity = 4;
    const double bounce_damping = 0.5;
    const double stop_threshold = 0.5;
    
    intro_velocity += gravity;
    intro_current_y += intro_velocity;
    
    // Bounce when hitting target position
    if (intro_current_y >= intro_target_y) {
      intro_current_y = intro_target_y;
      intro_velocity = -intro_velocity * bounce_damping;
      
      // Stop bouncing when velocity is small enough
      if (abs(intro_velocity) < stop_threshold) {
        intro_velocity = 0;
        intro_current_y = intro_target_y;
        intro_animating = false;
      }
    }
    
    animation_frame++;
  }

  // Handle go button (starts the level) with animated position
  if (selected_level_button != NULL) {
    int y_offset = intro_current_y - intro_target_y;
    Rectangle animated_go_rect(go_button_rect.x1, go_button_rect.y1 + y_offset,
                                go_button_rect.x2, go_button_rect.y2 + y_offset);
    
    if (animated_go_rect.overlap(DC->mouse)) {
      if (DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
        SC->play(levelmenu::button_selected_sound_path, ALLEGRO_PLAYMODE_ONCE);
        Player::getPlayer()->setrequest(Game::STATE::LEVEL);
        debug_log("<LevelMenu> Go button clicked, starting level %d\n", selected_level_button->level);
      }
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
        
        // Start drop animation
        intro_animating = true;
        intro_current_y = -500.0;
        intro_velocity = 0.0;
        animation_frame = 0;
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

  // Draw level intro if a level is selected
  if (selected_level_button != NULL) {
    // Draw dimmed overlay
    al_draw_filled_rectangle(0, 0, 1280, 720, al_map_rgba(0, 0, 0, 150));
    
    int intro_x = 300;
    int intro_y = (int)intro_current_y;
    al_draw_bitmap(level_intro_image, intro_x, intro_y, 0);
    
    // Draw level-specific intro text
    FontCenter *FC = FontCenter::get_instance();
    int level_index = selected_level_button->level - 1;
    if (level_index >= 0 && level_index < 6) {
      int text_x = intro_x + al_get_bitmap_width(level_intro_image) / 2;
      int text_y = intro_y + al_get_bitmap_height(level_intro_image) / 2 - 30;
      int line_spacing = 30;
      
      for (int i = 0; i < 3; i++) {
        if (strlen(levelmenu::level_intro_texts[level_index][i]) > 0) {
          al_draw_text(
            FC->courier_new[FontSize::MEDIUM],
            al_map_rgb(255, 255, 255),
            text_x,
            text_y + i * line_spacing,
            ALLEGRO_ALIGN_CENTER,
            levelmenu::level_intro_texts[level_index][i]
          );
        }
      }
    }
    
    // Calculate button positions relative to animated intro position
    int y_offset = intro_y - intro_target_y;
    int close_button_x = close_button_rect.x1;
    int close_button_y = close_button_rect.y1 + y_offset;
    int go_button_x = go_button_rect.x1;
    int go_button_y = go_button_rect.y1 + y_offset;
    
    // Create temporary rectangles for collision detection with animated positions
    Rectangle animated_close_rect(close_button_x, close_button_y,
                                   close_button_x + al_get_bitmap_width(close_button_image),
                                   close_button_y + al_get_bitmap_height(close_button_image));
    Rectangle animated_go_rect(go_button_x, go_button_y,
                                go_button_x + al_get_bitmap_width(go_button_image),
                                go_button_y + al_get_bitmap_height(go_button_image));
    
    // Draw close button at animated position
    if (animated_close_rect.overlap(DC->mouse)) {
      al_draw_bitmap(close_hover_button_image, close_button_x, close_button_y, 0);
    } else {
      al_draw_bitmap(close_button_image, close_button_x, close_button_y, 0);
    }
    
    // Draw go button at animated position
    if (animated_go_rect.overlap(DC->mouse)) {
      al_draw_bitmap(go_hover_button_image, go_button_x, go_button_y, 0);
    } else {
      al_draw_bitmap(go_button_image, go_button_x, go_button_y, 0);
    }
  }
}

void LevelMenu::end() {

}