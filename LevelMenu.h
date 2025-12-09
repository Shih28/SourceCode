#ifndef LEVELMENU_H_INCLUDED
#define LEVELMENU_H_INCLUDED

#include <optional>
#include <allegro5/allegro.h>
#include "data/ImageCenter.h"
#include "Game.h"
#include "GameModule.h"
#include "LevelButton.h"

class LevelMenu : public GameModule
{
private:
  ALLEGRO_BITMAP *background;
  ALLEGRO_BITMAP *button_image;
  ALLEGRO_BITMAP *button_hover_image;
  ALLEGRO_BITMAP *button_selected_image;
  ALLEGRO_BITMAP *level_intro_image;
  int levels_passed;
  std::optional<Game::STATE> next_state;
  LevelButton level_buttons[6];
  LevelButton* selected_level_button;
  bool last_mouse_state;
public:
  void init() override;
  void update() override;
  void draw() override;
  std::optional<Game::STATE> get_game_state_update_request() override;
};

#endif