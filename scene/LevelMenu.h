#ifndef LEVELMENU_H_INCLUDED
#define LEVELMENU_H_INCLUDED

#include <optional>
#include <allegro5/allegro.h>
#include "../shapes/Rectangle.h"
#include "Scene.h"
#include "LevelButton.h"

class LevelMenu : public Scene
{
private:
  ALLEGRO_BITMAP *background;
  ALLEGRO_BITMAP *button_image;
  ALLEGRO_BITMAP *button_hover_image;
  ALLEGRO_BITMAP *button_selected_image;
  ALLEGRO_BITMAP *level_intro_image;
  ALLEGRO_BITMAP *return_button_image;
  ALLEGRO_BITMAP *return_hover_button_image;
  ALLEGRO_BITMAP *close_button_image;
  ALLEGRO_BITMAP *close_hover_button_image;
  ALLEGRO_BITMAP *go_button_image;
  ALLEGRO_BITMAP *go_hover_button_image;
  int levels_passed;
  LevelButton level_buttons[6];
  LevelButton* selected_level_button;
  Rectangle return_button_rect;
  Rectangle close_button_rect;
  Rectangle go_button_rect;
  
  // Animation variables
  bool intro_animating;
  double intro_current_y;
  double intro_velocity;
  double intro_target_y;
  int animation_frame;
public:
  void init();
  void scene_init();
  void update();
  void draw();
  void end();

  static LevelMenu* getInstance(){
    static LevelMenu level_menu;
    return &level_menu;
  }
};

#endif