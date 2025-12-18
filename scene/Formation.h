#ifndef FORMATION_H_INCLUDED
#define FORMATION_H_INCLUDED

#include "Scene.h"
#include "../Monster.h"
#include "../shapes/Rectangle.h"
#include <vector>
#include <allegro5/allegro.h>

/**
 * @brief Formation scene where player selects up to 5 monsters for battle
 */
class Formation : public Scene
{
public:
  void init() override;
  void update() override;
  void draw() override;
  void end() override;

  void scene_init();

  static Formation *get()
  {
    static Formation formation;
    return &formation;
  }

private:
  Formation() : state(SELECTING), monster_selection_open(false), selected_slot(-1) {}

  enum STATE_F
  {
    SELECTING, // Selecting monsters
    READY      // Ready to go to battle
  };

  STATE_F state;

  // Selected monsters (up to 5, nullptr means empty slot)
  Monster *selected_monsters[5];

  // Monster selection popup state
  bool monster_selection_open;
  int selected_slot; // Which slot is being filled (-1 = none)

  // UI elements
  Rectangle slot_rects[5];       // The 5 formation slots
  Rectangle add_button_rects[5]; // The + buttons for empty slots
  Rectangle go_button_rect;      // The GO button
  Rectangle return_button_rect;    // Back to level menu button
  Rectangle cancel_button_rect;  // Cancel button in popup

  // Monster selection popup
  std::vector<Rectangle> monster_popup_rects;

  // Images
  ALLEGRO_BITMAP *background;
  ALLEGRO_BITMAP *slot_empty_image;
  ALLEGRO_BITMAP *go_button_image;
  ALLEGRO_BITMAP *go_button_hover_image;
  ALLEGRO_BITMAP *return_button_image;
  ALLEGRO_BITMAP *return_button_hover_image;
  ALLEGRO_BITMAP *popup_background;
  ALLEGRO_BITMAP *remove_button_image;
  ALLEGRO_BITMAP *remove_button_hover_image;

  // Helper functions
  void openMonsterSelection(int slot);
  void closeMonsterSelection();
  void selectMonster(Monster *monster);
  void removeMonster(int slot);
  int getSelectedMonsterCount() const;
  void drawMonsterSlots();
  void drawMonsterSelectionPopup();
  void drawGoButton();
};

#endif
