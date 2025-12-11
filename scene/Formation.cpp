#include "Formation.h"
#include "../Game.h"
#include "../Utils.h"
#include "../Player.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include "../shapes/Point.h"
#include "BattleField.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

namespace formation
{
  constexpr char background_image_path[] = "./assets/image/formation/backgound.png";
  constexpr char go_button_image_path[] = "./assets/image/formation/go.png";
  constexpr char go_button_hover_image_path[] = "./assets/image/formation/go_hover.png";
  constexpr char return_button_image_path[] = "./assets/image/formation/return.png";
  constexpr char return_button_hover_image_path[] = "./assets/image/formation/return_hover.png";
}

void Formation::init()
{
  state = SELECTING;
  monster_selection_open = false;
  selected_slot = -1;

  // Clear selected monsters
  for (int i = 0; i < 5; ++i)
  {
    selected_monsters[i] = nullptr;
  }

  // Load images
  ImageCenter *IC = ImageCenter::get_instance();
  background = IC->get(formation::background_image_path);
  go_button_image = IC->get(formation::go_button_image_path);
  go_button_hover_image = IC->get(formation::go_button_hover_image_path);
  return_button_image = IC->get(formation::return_button_image_path);
  return_button_hover_image = IC->get(formation::return_button_hover_image_path);

  // Setup slot positions (horizontal layout)
  int slot_width = 120;
  int slot_height = 120;
  int slot_spacing = 30;
  int start_x = (1280 - (5 * slot_width + 4 * slot_spacing)) / 2;
  int slot_y = 250;

  for (int i = 0; i < 5; ++i)
  {
    int x = start_x + i * (slot_width + slot_spacing);
    slot_rects[i] = Rectangle(x, slot_y, x + slot_width, slot_y + slot_height);

    // Add button in center of slot
    int add_btn_size = 40;
    int add_x = x + (slot_width - add_btn_size) / 2;
    int add_y = slot_y + (slot_height - add_btn_size) / 2;
    add_button_rects[i] = Rectangle(add_x, add_y, add_x + add_btn_size, add_y + add_btn_size);
  }

  // Setup go button (bottom center-right)
  int go_btn_width = al_get_bitmap_width(go_button_image);
  int go_btn_height = al_get_bitmap_height(go_button_image);
  int go_x = 640 + 50; // Slightly right of center
  int go_y = 580;
  go_button_rect = Rectangle(go_x, go_y, go_x + go_btn_width, go_y + go_btn_height);

  // Setup back button (bottom center-left)
  int back_btn_width = al_get_bitmap_width(return_button_image);
  int back_btn_height = al_get_bitmap_height(return_button_image);
  int back_x = 640 - back_btn_width - 50; // Slightly left of center
  int back_y = 580;
  return_button_rect = Rectangle(back_x, back_y, back_x + back_btn_width, back_y + back_btn_height);

  // Setup cancel button for popup
  cancel_button_rect = Rectangle(490, 630, 790, 680);
}

void Formation::scene_init()
{
  state = SELECTING;
  monster_selection_open = false;
  selected_slot = -1;
}

void Formation::update()
{
  auto DC = DataCenter::get_instance();
  Player *pl = Player::getPlayer();

  if (monster_selection_open)
  {
    // Handle monster selection popup

    // Cancel button
    if (cancel_button_rect.overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1])
    {
      closeMonsterSelection();
      return;
    }

    // Check if clicking on any monster in the popup
    auto &monsters = pl->getMonsters();
    int popup_start_x = 250;
    int popup_start_y = 150;
    int monster_width = 100;
    int monster_height = 100;
    int monsters_per_row = 7;
    int spacing = 20;

    for (size_t i = 0; i < monsters.size(); ++i)
    {
      int row = i / monsters_per_row;
      int col = i % monsters_per_row;
      int x = popup_start_x + col * (monster_width + spacing);
      int y = popup_start_y + row * (monster_height + spacing);

      Rectangle monster_rect(x, y, x + monster_width, y + monster_height);

      if (monster_rect.overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1])
      {
        selectMonster(&monsters[i]);
        closeMonsterSelection();
        return;
      }
    }
  }
  else
  {
    // Normal formation selection mode

    // Check add buttons for empty slots
    for (int i = 0; i < 5; ++i)
    {
      if (selected_monsters[i] == nullptr)
      {
        // Empty slot - check add button
        if (add_button_rects[i].overlap(DC->mouse) &&
            DC->mouse_state[1] && !DC->prev_mouse_state[1])
        {
          openMonsterSelection(i);
          return;
        }
      }
      else
      {
        // Filled slot - right click to remove
        if (slot_rects[i].overlap(DC->mouse) &&
            DC->mouse_state[2] && !DC->prev_mouse_state[2])
        {
          removeMonster(i);
          return;
        }
      }
    }

    // Check back button
    if (return_button_rect.overlap(DC->mouse) &&
        DC->mouse_state[1] && !DC->prev_mouse_state[1])
    {
      pl->setrequest(Game::STATE::LEVEL);
      return;
    }

    // Check go button (only if at least 1 monster selected)
    if (getSelectedMonsterCount() >= 0)
    {
      if (go_button_rect.overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1])
      {

        // Prepare monsters for battle
        std::vector<Monster *> battle_monsters;
        for (int i = 0; i < 5; ++i)
        {
          if (selected_monsters[i] != nullptr)
          {
            battle_monsters.push_back(selected_monsters[i]);
          }
        }

        BattleField::get()->setPlayerMonsters(battle_monsters);
        // TODO: Set enemy monsters based on level
        pl->setrequest(Game::STATE::BATTLE);
        return;
      }
    }

    // Back to level menu (ESC key)
    if (DC->key_state[ALLEGRO_KEY_ESCAPE] && !DC->prev_key_state[ALLEGRO_KEY_ESCAPE])
    {
      pl->setrequest(Game::STATE::LEVEL);
    }
  }
}

void Formation::draw()
{
  auto FC = FontCenter::get_instance();

  // Draw background
  al_draw_bitmap(background, 0, 0, ALLEGRO_ALIGN_LEFT);

  // Draw title
  if (FC->caviar_dreams[FontSize::LARGE])
  {
    al_draw_text(FC->caviar_dreams[FontSize::LARGE],
                 al_map_rgb(255, 255, 255),
                 640, 80,
                 ALLEGRO_ALIGN_CENTER,
                 "Select Your Team");
  }

  // Draw instruction
  if (FC->caviar_dreams[FontSize::SMALL])
  {
    al_draw_text(FC->caviar_dreams[FontSize::SMALL],
                 al_map_rgb(200, 200, 200),
                 640, 140,
                 ALLEGRO_ALIGN_CENTER,
                 "Choose up to 5 monsters - Right click to remove");
  }

  drawMonsterSlots();

  if (monster_selection_open)
  {
    drawMonsterSelectionPopup();
  }
  else
  {
    drawGoButton();
  }
}

void Formation::drawMonsterSlots()
{
  auto FC = FontCenter::get_instance();

  for (int i = 0; i < 5; ++i)
  {
    Rectangle &slot = slot_rects[i];

    // Draw slot background
    al_draw_filled_rectangle(slot.x1, slot.y1, slot.x2, slot.y2,
                             al_map_rgb(80, 80, 120));
    al_draw_rectangle(slot.x1, slot.y1, slot.x2, slot.y2,
                      al_map_rgb(150, 150, 200), 3);

    if (selected_monsters[i] == nullptr)
    {
      // Empty slot - draw + button
      Rectangle &add_btn = add_button_rects[i];

      auto DC = DataCenter::get_instance();
      bool hovering = add_btn.overlap(DC->mouse);
      ALLEGRO_COLOR btn_color = hovering ? al_map_rgb(100, 200, 100) : al_map_rgb(70, 150, 70);

      // Draw circle button
      int center_x = (add_btn.x1 + add_btn.x2) / 2;
      int center_y = (add_btn.y1 + add_btn.y2) / 2;
      int radius = (add_btn.x2 - add_btn.x1) / 2;

      al_draw_filled_circle(center_x, center_y, radius, btn_color);
      al_draw_circle(center_x, center_y, radius, al_map_rgb(255, 255, 255), 2);

      // Draw + symbol
      if (FC->caviar_dreams[FontSize::LARGE])
      {
        al_draw_text(FC->caviar_dreams[FontSize::LARGE],
                     al_map_rgb(255, 255, 255),
                     center_x, center_y - 25,
                     ALLEGRO_ALIGN_CENTER,
                     "+");
      }

      // Draw slot number
      if (FC->caviar_dreams[FontSize::SMALL])
      {
        char slot_text[16];
        snprintf(slot_text, sizeof(slot_text), "Slot %d", i + 1);
        al_draw_text(FC->caviar_dreams[FontSize::SMALL],
                     al_map_rgb(200, 200, 200),
                     (slot.x1 + slot.x2) / 2, slot.y2 + 10,
                     ALLEGRO_ALIGN_CENTER,
                     slot_text);
      }
    }
    else
    {
      // Filled slot - draw monster
      Monster *monster = selected_monsters[i];
      if (monster->getImg())
      {
        // Center monster image in slot
        int img_width = al_get_bitmap_width(monster->getImg());
        int img_height = al_get_bitmap_height(monster->getImg());
        int x = slot.x1 + ((slot.x2 - slot.x1) - img_width) / 2;
        int y = slot.y1 + ((slot.y2 - slot.y1) - img_height) / 2;

        al_draw_bitmap(monster->getImg(), x, y, 0);
      }

      // Draw species indicator
      if (FC->caviar_dreams[FontSize::SMALL])
      {
        const char *species_name = "";
        ALLEGRO_COLOR species_color = al_map_rgb(255, 255, 255);

        switch (monster->getSpecies())
        {
        case Monster::WATER:
          species_name = "WATER";
          species_color = al_map_rgb(100, 150, 255);
          break;
        case Monster::FIRE:
          species_name = "FIRE";
          species_color = al_map_rgb(255, 100, 50);
          break;
        case Monster::WIND:
          species_name = "WIND";
          species_color = al_map_rgb(150, 255, 150);
          break;
        case Monster::LIGHTNING:
          species_name = "LIGHTNING";
          species_color = al_map_rgb(255, 255, 100);
          break;
        default:
          species_name = "???";
          break;
        }

        al_draw_text(FC->caviar_dreams[FontSize::SMALL],
                     species_color,
                     (slot.x1 + slot.x2) / 2, slot.y2 + 10,
                     ALLEGRO_ALIGN_CENTER,
                     species_name);
      }
    }
  }
}

void Formation::drawMonsterSelectionPopup()
{
  auto FC = FontCenter::get_instance();
  auto DC = DataCenter::get_instance();
  Player *pl = Player::getPlayer();

  // Draw semi-transparent overlay
  al_draw_filled_rectangle(0, 0, 1280, 720, al_map_rgba(0, 0, 0, 180));

  // Draw popup background
  al_draw_filled_rectangle(140, 80, 1140, 690, al_map_rgb(40, 40, 80));
  al_draw_rectangle(140, 80, 1140, 690, al_map_rgb(150, 150, 200), 3);

  // Draw title
  if (FC->caviar_dreams[FontSize::LARGE])
  {
    al_draw_text(FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255), 640, 100, ALLEGRO_ALIGN_CENTER, "Choose a Monster");
  }

  // Draw monsters in grid
  auto &monsters = pl->getMonsters();
  int popup_start_x = 200;
  int popup_start_y = 160;
  int monster_width = 100;
  int monster_height = 100;
  int monsters_per_row = 9;
  int spacing = 15;

  for (size_t i = 0; i < monsters.size(); ++i)
  {
    int row = i / monsters_per_row;
    int col = i % monsters_per_row;
    int x = popup_start_x + col * (monster_width + spacing);
    int y = popup_start_y + row * (monster_height + spacing);

    Rectangle monster_rect(x, y, x + monster_width, y + monster_height);
    bool hovering = monster_rect.overlap(DC->mouse);

    // Check if monster is already selected
    bool already_selected = false;
    for (int j = 0; j < 5; ++j)
    {
      if (selected_monsters[j] == &monsters[i])
      {
        already_selected = true;
        break;
      }
    }

    // Draw monster slot
    ALLEGRO_COLOR bg_color;
    if (already_selected)
    {
      bg_color = al_map_rgb(100, 50, 50); // Red tint for already selected
    }
    else if (hovering)
    {
      bg_color = al_map_rgb(100, 100, 150);
    }
    else
    {
      bg_color = al_map_rgb(60, 60, 100);
    }

    al_draw_filled_rectangle(x, y, x + monster_width, y + monster_height, bg_color);
    al_draw_rectangle(x, y, x + monster_width, y + monster_height, al_map_rgb(150, 150, 200), 2);

    // Draw monster image
    if (monsters[i].getImg())
    {
      int img_width = al_get_bitmap_width(monsters[i].getImg());
      int img_height = al_get_bitmap_height(monsters[i].getImg());
      int img_x = x + (monster_width - img_width) / 2;
      int img_y = y + (monster_height - img_height) / 2;

      al_draw_bitmap(monsters[i].getImg(), img_x, img_y, 0);
    }

    // Draw "SELECTED" text if already selected
    if (already_selected && FC->caviar_dreams[FontSize::SMALL])
    {
      al_draw_text(FC->caviar_dreams[FontSize::SMALL], al_map_rgb(255, 100, 100), x + monster_width / 2, y + monster_height - 15, ALLEGRO_ALIGN_CENTER, "USED");
    }
  }

  // Draw cancel button
  bool cancel_hovering = cancel_button_rect.overlap(DC->mouse);
  ALLEGRO_COLOR cancel_color = cancel_hovering ? al_map_rgb(200, 100, 100) : al_map_rgb(150, 70, 70);

  al_draw_filled_rectangle(cancel_button_rect.x1, cancel_button_rect.y1,
                           cancel_button_rect.x2, cancel_button_rect.y2,
                           cancel_color);
  al_draw_rectangle(cancel_button_rect.x1, cancel_button_rect.y1,
                    cancel_button_rect.x2, cancel_button_rect.y2,
                    al_map_rgb(255, 255, 255), 2);

  if (FC->caviar_dreams[FontSize::MEDIUM])
  {
    al_draw_text(FC->caviar_dreams[FontSize::MEDIUM],
                 al_map_rgb(255, 255, 255),
                 (cancel_button_rect.x1 + cancel_button_rect.x2) / 2,
                 (cancel_button_rect.y1 + cancel_button_rect.y2) / 2 - 12,
                 ALLEGRO_ALIGN_CENTER,
                 "Cancel");
  }
}

void Formation::drawGoButton()
{
  auto DC = DataCenter::get_instance();
  auto FC = FontCenter::get_instance();

  int count = getSelectedMonsterCount();

  // Draw back button (always visible)
  bool hovering = return_button_rect.overlap(DC->mouse);
  ALLEGRO_BITMAP *return_image = hovering ? return_button_hover_image : return_button_image;

  al_draw_bitmap(return_image, return_button_rect.x1, return_button_rect.y1, 0);
  al_draw_bitmap(go_button_image, go_button_rect.x1, go_button_rect.y1, 0);

  if (count > 0)
  {
    // Draw go button
    bool hovering = go_button_rect.overlap(DC->mouse);
    ALLEGRO_BITMAP *btn_image = hovering ? go_button_hover_image : go_button_image;

    al_draw_bitmap(btn_image, go_button_rect.x1, go_button_rect.y1, 0);

    // Draw monster count
    if (FC->caviar_dreams[FontSize::MEDIUM])
    {
      char count_text[32];
      snprintf(count_text, sizeof(count_text), "%d Monster%s Selected", count, count > 1 ? "s" : "");
      al_draw_text(FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255), 640, 520, ALLEGRO_ALIGN_CENTER, count_text);
    }
  }
  else
  {
    if (FC->caviar_dreams[FontSize::MEDIUM])
    {
      al_draw_text(FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(200, 50, 50), 640, 520, ALLEGRO_ALIGN_CENTER, "Select at least 1 monster");
    }
  }
}

void Formation::openMonsterSelection(int slot)
{
  selected_slot = slot;
  monster_selection_open = true;
}

void Formation::closeMonsterSelection()
{
  monster_selection_open = false;
  selected_slot = -1;
}

void Formation::selectMonster(Monster *monster)
{
  if (selected_slot >= 0 && selected_slot < 5)
  {
    selected_monsters[selected_slot] = monster;
  }
}

void Formation::removeMonster(int slot)
{
  if (slot >= 0 && slot < 5)
  {
    selected_monsters[slot] = nullptr;
  }
}

int Formation::getSelectedMonsterCount() const
{
  int count = 0;
  for (int i = 0; i < 5; ++i)
  {
    if (selected_monsters[i] != nullptr)
    {
      count++;
    }
  }
  return count;
}

void Formation::end()
{
  for (int i = 0; i < 5; ++i)
  {
    selected_monsters[i] = nullptr;
  }
  monster_selection_open = false;
  selected_slot = -1;
}
