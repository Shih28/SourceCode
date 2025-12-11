#include "BattleField.h"
#include "../Game.h"
#include "../Utils.h"
#include "../Player.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include "../shapes/Rectangle.h"
#include "../shapes/Point.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <cstdio>

void BattleField::init()
{
  state = BATTLE_INIT;
  current_turn = 0;
  battle_timer = 0.0f;

  // Initialize pause animation
  pause_animating = false;
  pause_current_y = -400.0;
  pause_velocity = 0.0;
  pause_target_y = 200.0;
  show_quit_confirm = false;

  // Clear any existing battle monsters
  player_monsters.clear();
  enemy_monsters.clear();

  // Load player monsters from Player
  Player* pl = Player::getPlayer();
  auto& monsters = pl->getMonsters();
  
  int y_offset = MONSTER_Y_START;
  for (size_t i = 0; i < monsters.size() && i < 3; ++i) {
    auto battle_monster = std::make_unique<BattleMonster>(&monsters[i], true);
    battle_monster->setPosition(PLAYER_ZONE_X, y_offset);
    player_monsters.push_back(std::move(battle_monster));
    y_offset += 120;
  }

  // TODO: Load enemy monsters based on level/difficulty
  
  state = BATTLE_ONGOING;
}

void BattleField::setPlayerMonsters(std::vector<Monster*> monsters)
{
  player_monsters.clear();
  int y_offset = MONSTER_Y_START;
  for (auto* monster : monsters) {
    if (monster) {
      auto battle_monster = std::make_unique<BattleMonster>(monster, true);
      battle_monster->setPosition(PLAYER_ZONE_X, y_offset);
      player_monsters.push_back(std::move(battle_monster));
      y_offset += 120;
    }
  }
}

void BattleField::setEnemyMonsters(std::vector<Monster*> monsters)
{
  enemy_monsters.clear();
  int y_offset = MONSTER_Y_START;
  for (auto* monster : monsters) {
    if (monster) {
      auto battle_monster = std::make_unique<BattleMonster>(monster, false);
      battle_monster->setPosition(ENEMY_ZONE_X, y_offset);
      enemy_monsters.push_back(std::move(battle_monster));
      y_offset += 120;
    }
  }
}

void BattleField::update()
{
  auto DC = DataCenter::get_instance();
  Player *pl = Player::getPlayer();

  switch (state)
  {
  case BATTLE_INIT:
    // Initialization handled in init()
    state = BATTLE_ONGOING;
    break;

  case BATTLE_ONGOING:
    updateBattle();

    // Pause button (top right)
    if (Point(1200, 50).overlap(DC->mouse, 30) && DC->mouse_state[1] && !DC->prev_mouse_state[1])
    {
      state = BATTLE_PAUSED;
      // Start pause animation
      pause_animating = true;
      pause_current_y = -400.0;
      pause_velocity = 0.0;
    }

    checkBattleEnd();
    break;

  case BATTLE_PAUSED:
    updatePause();
    break;

  case BATTLE_WON:
    // Victory screen
    if (DC->key_state[ALLEGRO_KEY_ENTER] && !DC->prev_key_state[ALLEGRO_KEY_ENTER])
    {
      pl->setrequest(Game::STATE::LEVEL);
    }
    break;

  case BATTLE_LOST:
    // Defeat screen
    if (DC->key_state[ALLEGRO_KEY_ENTER] && !DC->prev_key_state[ALLEGRO_KEY_ENTER])
    {
      pl->setrequest(Game::STATE::MENU);
    }
    break;

  default:
    break;
  }
}

void BattleField::updateBattle()
{
  battle_timer += 1.0f / 60.0f; // Assuming 60 FPS

  // Update underlying monsters
  for (auto& battle_monster : player_monsters)
  {
    if (battle_monster && battle_monster->getMonster())
    {
      battle_monster->getMonster()->update();
      // Update status effects
      battle_monster->decrementStatusEffects();
    }
  }

  // Update enemy monsters
  for (auto& battle_monster : enemy_monsters)
  {
    if (battle_monster && battle_monster->getMonster())
    {
      battle_monster->getMonster()->update();
      // Update status effects
      battle_monster->decrementStatusEffects();
    }
  }

  // TODO: Implement battle logic
  // - Turn-based system
  // - Attack animations
  // - Damage calculations
  // - Use BattleMonster methods like takeDamage(), heal(), etc.
}

void BattleField::updatePause()
{
  auto DC = DataCenter::get_instance();
  Player *pl = Player::getPlayer();

  // Update pause box animation (drop without bounce)
  if (pause_animating)
  {
    const double gravity = 2.5;
    
    pause_velocity += gravity;
    pause_current_y += pause_velocity;
    
    // Stop when reaching target position (no bounce)
    if (pause_current_y >= pause_target_y)
    {
      pause_current_y = pause_target_y;
      pause_velocity = 0;
      pause_animating = false;
    }
  }

  // Close button position (on the pause box)
  int box_width = 600;
  int box_x = (FIELD_WIDTH - box_width) / 2;
  int box_y = (int)pause_current_y;
  Rectangle close_button_rect(box_x + box_width - 60, box_y + 10, box_x + box_width - 10, box_y + 60);

  // Handle close button click to resume
  if (close_button_rect.overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1])
  {
    state = BATTLE_ONGOING;
    pause_animating = false;
  }

  // Resume button (ESC key)
  if (DC->key_state[ALLEGRO_KEY_ESCAPE] && !DC->prev_key_state[ALLEGRO_KEY_ESCAPE])
  {
    if (show_quit_confirm)
    {
      show_quit_confirm = false; // Cancel quit confirmation
    }
    else
    {
      state = BATTLE_ONGOING;
      pause_animating = false;
    }
  }

  // Quit confirmation dialog
  if (show_quit_confirm)
  {
    // Confirm dialog dimensions
    int confirm_width = 500;
    int confirm_height = 200;
    int confirm_x = (FIELD_WIDTH - confirm_width) / 2;
    int confirm_y = (FIELD_HEIGHT - confirm_height) / 2;
    
    // Yes button
    Rectangle yes_button_rect(confirm_x + 50, confirm_y + confirm_height - 70, 
                             confirm_x + 200, confirm_y + confirm_height - 20);
    
    // No button
    Rectangle no_button_rect(confirm_x + confirm_width - 200, confirm_y + confirm_height - 70, 
                            confirm_x + confirm_width - 50, confirm_y + confirm_height - 20);
    
    // Handle Yes button click
    if (yes_button_rect.overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1])
    {
      pl->setrequest(Game::STATE::MENU);
      show_quit_confirm = false;
    }
    
    // Handle No button click
    if (no_button_rect.overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1])
    {
      show_quit_confirm = false;
    }
  }
  else
  {
    // Return to menu button (Q key) - show confirmation
    if (DC->key_state[ALLEGRO_KEY_Q] && !DC->prev_key_state[ALLEGRO_KEY_Q])
    {
      show_quit_confirm = true;
    }
  }
}

void BattleField::checkBattleEnd()
{
  bool all_player_defeated = true;
  bool all_enemy_defeated = true;

  // Check if all player monsters are defeated
  for (auto& battle_monster : player_monsters)
  {
    if (battle_monster && battle_monster->isAlive())
    {
      all_player_defeated = false;
      break;
    }
  }

  // Check if all enemy monsters are defeated
  for (auto& battle_monster : enemy_monsters)
  {
    if (battle_monster && battle_monster->isAlive())
    {
      all_enemy_defeated = false;
      break;
    }
  }

  if (all_enemy_defeated && !enemy_monsters.empty())
  {
    state = BATTLE_WON;
  }
  else if (all_player_defeated && !player_monsters.empty())
  {
    state = BATTLE_LOST;
  }
}

void BattleField::draw()
{
  auto FC = FontCenter::get_instance();

  // Draw background
  // TODO: Load and draw battle background image
  al_clear_to_color(al_map_rgb(100, 150, 200)); // Temporary sky blue background

  // Draw battlefield ground
  al_draw_filled_rectangle(0, 500, FIELD_WIDTH, FIELD_HEIGHT,
                           al_map_rgb(100, 200, 100)); // Green ground

  switch (state)
  {
  case BATTLE_ONGOING:
  case BATTLE_PAUSED:
    drawMonsters();
    drawUI();
    break;

  case BATTLE_WON:
    drawMonsters();
    drawUI();
    // Victory message
    if (FC->caviar_dreams[FontSize::LARGE])
    {
      al_draw_text(FC->caviar_dreams[FontSize::LARGE],
                   al_map_rgb(255, 215, 0),
                   FIELD_WIDTH / 2, 200,
                   ALLEGRO_ALIGN_CENTER,
                   "VICTORY!");
      al_draw_text(FC->caviar_dreams[FontSize::MEDIUM],
                   al_map_rgb(255, 255, 255),
                   FIELD_WIDTH / 2, 280,
                   ALLEGRO_ALIGN_CENTER,
                   "Press ENTER to continue");
    }
    break;

  case BATTLE_LOST:
    drawMonsters();
    drawUI();
    // Defeat message
    if (FC->caviar_dreams[FontSize::LARGE])
    {
      al_draw_text(FC->caviar_dreams[FontSize::LARGE],
                   al_map_rgb(200, 0, 0),
                   FIELD_WIDTH / 2, 200,
                   ALLEGRO_ALIGN_CENTER,
                   "DEFEAT");
      al_draw_text(FC->caviar_dreams[FontSize::MEDIUM],
                   al_map_rgb(255, 255, 255),
                   FIELD_WIDTH / 2, 280,
                   ALLEGRO_ALIGN_CENTER,
                   "Press ENTER to return");
    }
    break;

  default:
    break;
  }

  // Draw pause box with animation
  if (state == BATTLE_PAUSED)
  {
    drawPauseBox();
    
    // Draw quit confirmation dialog on top if shown
    if (show_quit_confirm)
    {
      drawQuitConfirm();
    }
  }
}

void BattleField::drawMonsters()
{
  auto FC = FontCenter::get_instance();
  
  // Draw player monsters (left side)
  for (size_t i = 0; i < player_monsters.size(); ++i)
  {
    auto& battle_monster = player_monsters[i];
    if (battle_monster && battle_monster->getMonster() && battle_monster->getMonster()->getImg())
    {
      int x = battle_monster->getPositionX();
      int y = battle_monster->getPositionY();
      
      al_draw_bitmap(battle_monster->getMonster()->getImg(), x, y, 0);
      
      // Draw HP bar
      int bar_width = 80;
      int bar_height = 8;
      int bar_x = x;
      int bar_y = y - 15;
      
      // Background bar
      al_draw_filled_rectangle(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, 
                              al_map_rgb(50, 50, 50));
      
      // HP bar (green if HP > 50%, yellow if > 25%, red otherwise)
      float hp_percent = (float)battle_monster->getCurrentHP() / battle_monster->getMaxHP();
      int hp_bar_width = (int)(bar_width * hp_percent);
      ALLEGRO_COLOR hp_color;
      if (hp_percent > 0.5f) {
        hp_color = al_map_rgb(0, 200, 0); // Green
      } else if (hp_percent > 0.25f) {
        hp_color = al_map_rgb(200, 200, 0); // Yellow
      } else {
        hp_color = al_map_rgb(200, 0, 0); // Red
      }
      al_draw_filled_rectangle(bar_x, bar_y, bar_x + hp_bar_width, bar_y + bar_height, hp_color);
      
      // HP text
      if (FC->caviar_dreams[FontSize::SMALL]) {
        char hp_text[32];
        snprintf(hp_text, sizeof(hp_text), "%d/%d", 
                battle_monster->getCurrentHP(), battle_monster->getMaxHP());
        al_draw_text(FC->caviar_dreams[FontSize::SMALL],
                   al_map_rgb(255, 255, 255),
                   bar_x + bar_width / 2, bar_y - 15,
                   ALLEGRO_ALIGN_CENTER,
                   hp_text);
      }
      
      // Draw status effects
      int status_y = y + 100;
      if (battle_monster->isStunned()) {
        al_draw_text(FC->caviar_dreams[FontSize::SMALL],
                   al_map_rgb(150, 150, 255),
                   x, status_y,
                   ALLEGRO_ALIGN_LEFT,
                   "STUN");
        status_y += 15;
      }
      if (battle_monster->isPoisoned()) {
        al_draw_text(FC->caviar_dreams[FontSize::SMALL],
                   al_map_rgb(150, 0, 150),
                   x, status_y,
                   ALLEGRO_ALIGN_LEFT,
                   "POISON");
      }
    }
  }

  // Draw enemy monsters (right side)
  for (size_t i = 0; i < enemy_monsters.size(); ++i)
  {
    auto& battle_monster = enemy_monsters[i];
    if (battle_monster && battle_monster->getMonster() && battle_monster->getMonster()->getImg())
    {
      int x = battle_monster->getPositionX();
      int y = battle_monster->getPositionY();
      
      al_draw_bitmap(battle_monster->getMonster()->getImg(), x, y, ALLEGRO_FLIP_HORIZONTAL);
      
      // Draw HP bar
      int bar_width = 80;
      int bar_height = 8;
      int bar_x = x;
      int bar_y = y - 15;
      
      // Background bar
      al_draw_filled_rectangle(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, 
                              al_map_rgb(50, 50, 50));
      
      // HP bar
      float hp_percent = (float)battle_monster->getCurrentHP() / battle_monster->getMaxHP();
      int hp_bar_width = (int)(bar_width * hp_percent);
      ALLEGRO_COLOR hp_color;
      if (hp_percent > 0.5f) {
        hp_color = al_map_rgb(0, 200, 0);
      } else if (hp_percent > 0.25f) {
        hp_color = al_map_rgb(200, 200, 0);
      } else {
        hp_color = al_map_rgb(200, 0, 0);
      }
      al_draw_filled_rectangle(bar_x, bar_y, bar_x + hp_bar_width, bar_y + bar_height, hp_color);
      
      // HP text
      if (FC->caviar_dreams[FontSize::SMALL]) {
        char hp_text[32];
        snprintf(hp_text, sizeof(hp_text), "%d/%d", 
                battle_monster->getCurrentHP(), battle_monster->getMaxHP());
        al_draw_text(FC->caviar_dreams[FontSize::SMALL],
                   al_map_rgb(255, 255, 255),
                   bar_x + bar_width / 2, bar_y - 15,
                   ALLEGRO_ALIGN_CENTER,
                   hp_text);
      }
      
      // Draw status effects
      int status_y = y + 100;
      if (battle_monster->isStunned()) {
        al_draw_text(FC->caviar_dreams[FontSize::SMALL],
                   al_map_rgb(150, 150, 255),
                   x, status_y,
                   ALLEGRO_ALIGN_LEFT,
                   "STUN");
        status_y += 15;
      }
      if (battle_monster->isPoisoned()) {
        al_draw_text(FC->caviar_dreams[FontSize::SMALL],
                   al_map_rgb(150, 0, 150),
                   x, status_y,
                   ALLEGRO_ALIGN_LEFT,
                   "POISON");
      }
    }
  }
}

void BattleField::drawUI()
{
  auto FC = FontCenter::get_instance();

  // Draw pause button (top right)
  al_draw_filled_circle(1200, 50, 30, al_map_rgb(200, 200, 200));
  if (FC->caviar_dreams[FontSize::SMALL])
  {
    al_draw_text(FC->caviar_dreams[FontSize::SMALL],
                 al_map_rgb(0, 0, 0),
                 1200, 40,
                 ALLEGRO_ALIGN_CENTER,
                 "||");
  }

  // Draw turn counter
  if (FC->caviar_dreams[FontSize::MEDIUM])
  {
    char turn_text[32];
    snprintf(turn_text, sizeof(turn_text), "Turn: %d", current_turn);
    al_draw_text(FC->caviar_dreams[FontSize::MEDIUM],
                 al_map_rgb(255, 255, 255),
                 FIELD_WIDTH / 2, 20,
                 ALLEGRO_ALIGN_CENTER,
                 turn_text);
  }

  // TODO: Draw HP bars, action buttons, skill icons, etc.
}

void BattleField::drawPauseBox()
{
  auto FC = FontCenter::get_instance();
  auto DC = DataCenter::get_instance();

  // Draw semi-transparent overlay on entire screen
  al_draw_filled_rectangle(0, 0, FIELD_WIDTH, FIELD_HEIGHT,
                           al_map_rgba(0, 0, 0, 128));

  // Pause box dimensions
  int box_width = 600;
  int box_height = 300;
  int box_x = (FIELD_WIDTH - box_width) / 2;
  int box_y = (int)pause_current_y;

  // Draw pause box background
  al_draw_filled_rectangle(box_x, box_y, box_x + box_width, box_y + box_height,
                           al_map_rgb(40, 40, 80));
  
  // Draw box border
  al_draw_rectangle(box_x, box_y, box_x + box_width, box_y + box_height,
                   al_map_rgb(150, 150, 200), 4);

  // Draw close button (X) at top right of box
  Rectangle close_button_rect(box_x + box_width - 60, box_y + 10, box_x + box_width - 10, box_y + 60);
  bool close_hovering = close_button_rect.overlap(DC->mouse);
  
  ALLEGRO_COLOR close_color = close_hovering ? 
    al_map_rgb(255, 100, 100) : al_map_rgb(200, 50, 50);
  
  al_draw_filled_rectangle(close_button_rect.x1, close_button_rect.y1,
                          close_button_rect.x2, close_button_rect.y2,
                          close_color);
  al_draw_rectangle(close_button_rect.x1, close_button_rect.y1,
                   close_button_rect.x2, close_button_rect.y2,
                   al_map_rgb(255, 255, 255), 2);
  
  // Draw X symbol
  if (FC->caviar_dreams[FontSize::LARGE])
  {
    al_draw_text(FC->caviar_dreams[FontSize::LARGE],
                al_map_rgb(255, 255, 255),
                (close_button_rect.x1 + close_button_rect.x2) / 2,
                (close_button_rect.y1 + close_button_rect.y2) / 2 - 18,
                ALLEGRO_ALIGN_CENTER,
                "X");
  }

  // Draw "GAME PAUSED" title
  if (FC->caviar_dreams[FontSize::LARGE])
  {
    al_draw_text(FC->caviar_dreams[FontSize::LARGE],
                al_map_rgb(255, 215, 0),
                box_x + box_width / 2,
                box_y + 80,
                ALLEGRO_ALIGN_CENTER,
                "GAME PAUSED");
  }

  // Draw instructions
  if (FC->caviar_dreams[FontSize::MEDIUM])
  {
    al_draw_text(FC->caviar_dreams[FontSize::MEDIUM],
                al_map_rgb(200, 200, 200),
                box_x + box_width / 2,
                box_y + 160,
                ALLEGRO_ALIGN_CENTER,
                "Click X or press ESC to resume");
    
    al_draw_text(FC->caviar_dreams[FontSize::MEDIUM],
                al_map_rgb(200, 200, 200),
                box_x + box_width / 2,
                box_y + 200,
                ALLEGRO_ALIGN_CENTER,
                "Press Q to quit to menu");
  }
}

void BattleField::drawQuitConfirm()
{
  FontCenter* FC = FontCenter::get_instance();
  DataCenter* DC = DataCenter::get_instance();
  
  // Draw darker overlay on top of pause box
  al_draw_filled_rectangle(0, 0, FIELD_WIDTH, FIELD_HEIGHT,
                          al_map_rgba(0, 0, 0, 128));
  
  // Confirmation dialog dimensions
  const int confirm_width = 500;
  const int confirm_height = 200;
  const int confirm_x = (FIELD_WIDTH - confirm_width) / 2;
  const int confirm_y = (FIELD_HEIGHT - confirm_height) / 2;
  
  // Draw confirmation box
  al_draw_filled_rectangle(confirm_x, confirm_y,
                          confirm_x + confirm_width, confirm_y + confirm_height,
                          al_map_rgb(30, 30, 60));
  al_draw_rectangle(confirm_x, confirm_y,
                   confirm_x + confirm_width, confirm_y + confirm_height,
                   al_map_rgb(255, 255, 255), 3);
  
  // Draw title
  if (FC->caviar_dreams[FontSize::LARGE])
  {
    al_draw_text(FC->caviar_dreams[FontSize::LARGE],
                al_map_rgb(255, 255, 255),
                confirm_x + confirm_width / 2,
                confirm_y + 30,
                ALLEGRO_ALIGN_CENTER,
                "Are you sure you want to quit?");
  }
  
  // Button dimensions
  const int button_width = 150;
  const int button_height = 50;
  const int yes_x = confirm_x + 50;
  const int no_x = confirm_x + confirm_width - 200;
  const int button_y = confirm_y + confirm_height - 70;
  
  Rectangle yes_button(yes_x, button_y, yes_x + button_width, button_y + button_height);
  Rectangle no_button(no_x, button_y, no_x + button_width, button_y + button_height);
  
  Point mouse(DC->mouse.x, DC->mouse.y);
  bool yes_hovering = yes_button.overlap(mouse);
  bool no_hovering = no_button.overlap(mouse);
  
  // Draw Yes button (green)
  ALLEGRO_COLOR yes_color = yes_hovering ? 
    al_map_rgb(100, 200, 100) : al_map_rgb(50, 150, 50);
  
  al_draw_filled_rectangle(yes_button.x1, yes_button.y1,
                          yes_button.x2, yes_button.y2,
                          yes_color);
  al_draw_rectangle(yes_button.x1, yes_button.y1,
                   yes_button.x2, yes_button.y2,
                   al_map_rgb(255, 255, 255), 2);
  
  if (FC->caviar_dreams[FontSize::MEDIUM])
  {
    al_draw_text(FC->caviar_dreams[FontSize::MEDIUM],
                al_map_rgb(255, 255, 255),
                yes_x + button_width / 2,
                button_y + 15,
                ALLEGRO_ALIGN_CENTER,
                "Yes");
  }
  
  // Draw No button (red)
  ALLEGRO_COLOR no_color = no_hovering ? 
    al_map_rgb(255, 100, 100) : al_map_rgb(200, 50, 50);
  
  al_draw_filled_rectangle(no_button.x1, no_button.y1,
                          no_button.x2, no_button.y2,
                          no_color);
  al_draw_rectangle(no_button.x1, no_button.y1,
                   no_button.x2, no_button.y2,
                   al_map_rgb(255, 255, 255), 2);
  
  if (FC->caviar_dreams[FontSize::MEDIUM])
  {
    al_draw_text(FC->caviar_dreams[FontSize::MEDIUM],
                al_map_rgb(255, 255, 255),
                no_x + button_width / 2,
                button_y + 15,
                ALLEGRO_ALIGN_CENTER,
                "No");
  }
}

void BattleField::end()
{
  // Clean up battle resources
  player_monsters.clear();
  enemy_monsters.clear();
  current_turn = 0;
  battle_timer = 0.0f;
  state = BATTLE_INIT;
}
