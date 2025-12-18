#include "BattleField.h"
#include "LevelMenu.h"
#include "../Monster.h"
#include "../Game.h"
#include "../Utils.h"
#include "../Player.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include "../data/SoundCenter.h"
#include "../shapes/Rectangle.h"
#include "../shapes/Point.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "../single_include/nlohmann/json.hpp"

using json = nlohmann::json;

void BattleField::init()
{
  state = BATTLE_INIT;
  current_turn = 1;
  battle_timer = 0.0f;
  current_level = 0;  // Initialize to 0, will be set by setLevel()
  
  // Initialize action selection
  player_actions.clear();
  current_selecting_monster = 0;
  waiting_for_action_selection = false;
  selected_target = nullptr;
  current_executing_action = 0;
  action_selection_delay_timer = 0.0f;
  last_hover_section = -1;  // No hover initially

  // Initialize monster walk animation frame count
  monster_walk_frame_count = 0;
  
  // Initialize round announcement animation
  show_round_announce = true;
  round_announce_x = -500.0f;
  round_announce_timer = 0.0f;
  round_phase = FLYING_IN;
  attack_delay_timer = 0.0f;

  // Initialize pause animation
  pause_animating = false;
  pause_current_y = -400.0;
  pause_velocity = 0.0;
  pause_target_y = 200.0;
  show_quit_confirm = false;

  // Load images
  auto IC = ImageCenter::get_instance();
  background_img = IC->get("./assets/image/battlefield/background.png");
  actionbar_img = IC->get("./assets/image/battlefield/actionbar.png");
  actionbar_attack_img = IC->get("./assets/image/battlefield/actionbar_attack.png");
  actionbar_abality_img = IC->get("./assets/image/battlefield/actionbar_abality.png");
  actionbar_ultimate_img = IC->get("./assets/image/battlefield/actionbar_ultimate.png");

  // Clear any existing battle monsters
  player_monsters.clear();
  enemy_monsters.clear();

  // Load player monsters from Player
  Player* pl = Player::getPlayer();
  auto& monsters = pl->getMonsters();
  
  // Two-line formation for player monsters
  // First line (closer to enemy): monsters 0, 1
  // Second line (farther from enemy): monsters 3, 2, 4
  const int FRONT_LINE_X = 250;  // Closer to enemy
  const int BACK_LINE_X = 100;   // Farther from enemy
  const int LINE_Y_POSITIONS[3] = {200, 320, 440};  // Top, Middle, Bottom
  
  for (size_t i = 0; i < monsters.size() && i < 5; ++i) {
    auto battle_monster = std::make_unique<BattleMonster>(&monsters[i], true);
    
    int x, y;
    if (i == 0) {  // 1st monster: front line, upper
      x = FRONT_LINE_X;
      y = LINE_Y_POSITIONS[0];
    } else if (i == 1) {  // 2nd monster: front line, lower
      x = FRONT_LINE_X;
      y = LINE_Y_POSITIONS[1];
    } else if (i == 2) {  // 3rd monster: back line, middle
      x = BACK_LINE_X;
      y = LINE_Y_POSITIONS[1];
    } else if (i == 3) {  // 4th monster: back line, top
      x = BACK_LINE_X;
      y = LINE_Y_POSITIONS[0];
    } else {  // 5th monster: back line, bottom
      x = BACK_LINE_X;
      y = LINE_Y_POSITIONS[2];
    }
    
    battle_monster->setPosition(x, y);
    player_monsters.push_back(std::move(battle_monster));
  }

  // Load enemy monsters for each level
  std::ifstream f("./assets/config/battlefield/level_monsters.json");
  json j = json::parse(f);
  for (int i = 0; i < 6; i++) {
    for (auto b : j["level_monsters"][i]["monsters"]) {  
      Monster *m = new Monster();
      m->setType((Monster::TYPE_M)b["type"].get<int>());
      m->setSpecies((Monster::SPECIES_M)b["species"].get<int>());
      level_monsters[i].push_back(m);
    }
  }
  
  state = BATTLE_ONGOING;
}

void BattleField::scene_init() {
  state = BATTLE_INIT;
  current_turn = 1;
  battle_timer = 0.0f;
  // Don't reset current_level here - it's set by setLevel() before scene_init()
  
  // Initialize action selection
  player_actions.clear();
  current_selecting_monster = 0;
  waiting_for_action_selection = false;
  selected_target = nullptr;
  current_executing_action = 0;
  action_selection_delay_timer = 0.0f;
  
  // Initialize round announcement animation
  show_round_announce = true;
  round_announce_x = -500.0f;
  round_announce_timer = 0.0f;
  round_phase = FLYING_IN;
  attack_delay_timer = 0.0f;

  // Initialize pause animation
  pause_animating = false;
  pause_current_y = -400.0;
  pause_velocity = 0.0;
  pause_target_y = 200.0;
  show_quit_confirm = false;
  
  // Initialize attack animation
  attack_anim_state = ANIM_NONE;
  attacking_monster = nullptr;
  attack_target = nullptr;
  attacker_original_x = 0;
  attacker_original_y = 0;
  attacker_current_x = 0;
  attacker_current_y = 0;
  attack_anim_timer = 0.0f;
  
  // Clear damage displays
  damage_displays.clear();
  
  // Reset monster walk animation
  monster_walk_frame_count = 0;
  
  // Reset hover section
  last_hover_section = -1;
}

void BattleField::setPlayerMonsters(std::vector<Monster*> monsters)
{
  player_monsters.clear();
  
  // Two-line formation for player monsters
  const int FRONT_LINE_X = 250;  // Closer to enemy
  const int BACK_LINE_X = 100;   // Farther from enemy
  const int LINE_Y_POSITIONS[5] = {260, 380, 320, 200, 440};  // Top, Middle, Bottom
  
  for (size_t i = 0; i < monsters.size() && i < 5; ++i) {
    if (monsters[i]) {
      auto battle_monster = std::make_unique<BattleMonster>(monsters[i], true);
      
      int x, y;
      if (i == 0) {  // 1st monster: front line, upper
        x = FRONT_LINE_X;
        y = LINE_Y_POSITIONS[0];
      } else if (i == 1) {  // 2nd monster: front line, lower
        x = FRONT_LINE_X;
        y = LINE_Y_POSITIONS[1];
      } else if (i == 2) {  // 3rd monster: back line, middle
        x = BACK_LINE_X;
        y = LINE_Y_POSITIONS[2];
      } else if (i == 3) {  // 4th monster: back line, top
        x = BACK_LINE_X;
        y = LINE_Y_POSITIONS[3];
      } else {  // 5th monster: back line, bottom
        x = BACK_LINE_X;
        y = LINE_Y_POSITIONS[4];
      }
      
      battle_monster->setPosition(x, y);
      player_monsters.push_back(std::move(battle_monster));
    }
  }
}

void BattleField::setLevel(int level)
{
  // Store current level
  current_level = level;
  
  // Load emeny monsters based on level
  enemy_monsters.clear();
  
  // Two-line formation for enemy monsters (mirrored from player)
  const int FRONT_LINE_X = 930;  // Closer to player
  const int BACK_LINE_X = 1080;   // Farther from player
  const int LINE_Y_POSITIONS[5] = {260, 380, 320, 200, 440};  // Top, Middle, Bottom
  
  for (size_t i = 0; i < level_monsters[level - 1].size() && i < 5; ++i) {
    if (level_monsters[level - 1][i]) {
      auto battle_monster = std::make_unique<BattleMonster>(level_monsters[level - 1][i], false);
      
      int x, y;
      if (i == 0) {  // 1st monster: front line, upper
        x = FRONT_LINE_X;
        y = LINE_Y_POSITIONS[0];
      } else if (i == 1) {  // 2nd monster: front line, lower
        x = FRONT_LINE_X;
        y = LINE_Y_POSITIONS[1];
      } else if (i == 2) {  // 3rd monster: back line, middle
        x = BACK_LINE_X;
        y = LINE_Y_POSITIONS[2];
      } else if (i == 3) {  // 4th monster: back line, top
        x = BACK_LINE_X;
        y = LINE_Y_POSITIONS[3];
      } else {  // 5th monster: back line, bottom
        x = BACK_LINE_X;
        y = LINE_Y_POSITIONS[4];
      }
      
      battle_monster->setPosition(x, y);
      enemy_monsters.push_back(std::move(battle_monster));
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
    break;

  case BATTLE_PAUSED:
    updatePause();
    break;

  case BATTLE_WON:
    // Continue updating damage displays even after winning
    updateDamageDisplays(1.0f / 60.0f);
    
    // Victory screen
    if (DC->key_state[ALLEGRO_KEY_ENTER] && !DC->prev_key_state[ALLEGRO_KEY_ENTER])
    {
      pl->setrequest(Game::STATE::LEVEL);
    }
    break;

  case BATTLE_LOST:
    // Continue updating damage displays even after losing
    updateDamageDisplays(1.0f / 60.0f);
    
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
  const float FPS = 60.0f;
  const float DELTA_TIME = 1.0f / FPS;
  auto DC = DataCenter::get_instance();
  
  // Check if any ultimate video is playing - if so, only update videos
  bool video_is_playing = false;
  for (auto& battle_monster : player_monsters)
  {
    if (battle_monster && battle_monster->isVideoPlaying())
    {
      video_is_playing = true;
      break;
    }
  }
  if (!video_is_playing)
  {
    for (auto& battle_monster : enemy_monsters)
    {
      if (battle_monster && battle_monster->isVideoPlaying())
      {
        video_is_playing = true;
        break;
      }
    }
  }
  
  // Update video states for all monsters
  for (auto& battle_monster : player_monsters)
  {
    if (battle_monster)
    {
      battle_monster->updateVideo();
    }
  }
  for (auto& battle_monster : enemy_monsters)
  {
    if (battle_monster)
    {
      battle_monster->updateVideo();
    }
  }
  
  // If video is playing, pause all battle logic
  if (video_is_playing)
  {
    return;
  }
  
  // Update underlying monsters (only when no video playing)
  for (auto& battle_monster : player_monsters)
  {
    if (battle_monster && battle_monster->getMonster())
    {
      battle_monster->getMonster()->update();
      battle_monster->decrementStatusEffects();
    }
  }

  for (auto& battle_monster : enemy_monsters)
  {
    if (battle_monster && battle_monster->getMonster())
    {
      battle_monster->getMonster()->update();
      battle_monster->decrementStatusEffects();
    }
  }
  
  // Update damage displays
  updateDamageDisplays(DELTA_TIME);

  // Handle round announcement animation
  if (show_round_announce)
  {
    const float FLY_SPEED = 30.0f;
    const float PAUSE_DURATION = 0.5f;
    const float CENTER_X = FIELD_WIDTH / 2.0f;
    
    switch (round_phase)
    {
      case FLYING_IN:
        round_announce_x += FLY_SPEED;
        if (round_announce_x >= CENTER_X)
        {
          round_announce_x = CENTER_X;
          round_phase = PAUSED;
          round_announce_timer = 0.0f;
        }
        break;
        
      case PAUSED:
        round_announce_timer += DELTA_TIME;
        if (round_announce_timer >= PAUSE_DURATION)
        {
          round_phase = FLYING_OUT;
        }
        break;
        
      case FLYING_OUT:
        round_announce_x += FLY_SPEED;
        if (round_announce_x >= FIELD_WIDTH + 500)
        {
          show_round_announce = false;
          round_phase = SELECTING_ACTIONS;
          player_actions.clear();
          current_selecting_monster = 0;
          waiting_for_action_selection = true;
        }
        break;
        
      default:
        break;
    }
    return;
  }

  // Action selection phase
  if (round_phase == SELECTING_ACTIONS && waiting_for_action_selection)
  {
    // Handle delay between monster selections
    if (action_selection_delay_timer > 0.0f)
    {
      action_selection_delay_timer -= DELTA_TIME;
      return;
    }
    
    // Check if all player monsters have selected actions
    if (current_selecting_monster >= (int)player_monsters.size())
    {
      // All actions selected, start execution
      waiting_for_action_selection = false;
      round_phase = EXECUTING_ACTIONS;
      current_executing_action = 0;
      attack_delay_timer = 0.0f;
      return;
    }
    
    // Handle action button clicks for current monster
    if (current_selecting_monster < (int)player_monsters.size())
    {
      auto& current_monster = player_monsters[current_selecting_monster];
      if (!current_monster || !current_monster->isAlive() || current_monster->isStunned())
      {
        // Skip this monster
        current_selecting_monster++;
        return;
      }
      
      // Action button click handling
      if (DC->mouse_state[1] && !DC->prev_mouse_state[1])
      {
        // Three equal sections from left to right: Attack, Skill, Ultimate
        Rectangle attack_button(55, 500, 450, 720);
        Rectangle skill_button(450, 500, 830, 720);
        Rectangle ultimate_button(830, 500, 1225, 720);
        
        Point mouse(DC->mouse.x, DC->mouse.y);
        
        // Find the first alive enemy as target
        std::vector<BattleMonster*> alive_enemies;
        for (auto& enemy : enemy_monsters)
        {
          if (enemy && enemy->isAlive())
          {
            alive_enemies.push_back(enemy.get());
          }
        }
        
        if (!alive_enemies.empty())
        {
          // Select first enemy in list
          BattleMonster* target = alive_enemies[0];
          
          MonsterAction action;
          action.monster = current_monster.get();
          action.target = target;
          
          bool action_selected = false;
          
          if (attack_button.overlap(mouse))
          {
            action.action = ATTACK;
            action_selected = true;
          }
          else if (skill_button.overlap(mouse))
          {
            action.action = ABILITY;
            action_selected = true;
          }
          else if (ultimate_button.overlap(mouse))
          {
            action.action = ULTIMATE_SKILL;
            action_selected = true;
          }
          
          // If an action was selected, add it and move to next monster
          if (action_selected)
          {
            player_actions.push_back(action);
            current_selecting_monster++;
            action_selection_delay_timer = 0.3f;  // 0.3 second delay before next selection
          }
        }
      }
    }
    return;
  }

  // Action execution phase
  if (round_phase == EXECUTING_ACTIONS)
  {
    const float ATTACK_DELAY = 0.5f;  // Delay between different monster attacks
    
    // Handle attack animation
    if (attack_anim_state != ANIM_NONE)
    {
      updateAttackAnimation(DELTA_TIME);
      return;
    }
    
    // Handle delay between attacks
    if (attack_delay_timer > 0.0f)
    {
      attack_delay_timer -= DELTA_TIME;
      return;
    }
    
    // Execute player actions
    if (current_executing_action < (int)player_actions.size())
    {
      startAttackAnimation(player_actions[current_executing_action]);
      return;
    }
    
    // Execute enemy actions (AI-controlled)
    int enemy_index = current_executing_action - (int)player_actions.size();
    if (enemy_index < (int)enemy_monsters.size())
    {
      auto& attacker = enemy_monsters[enemy_index];
      if (attacker && attacker->isAlive() && !attacker->isStunned())
      {
        std::vector<BattleMonster*> alive_players;
        for (auto& player : player_monsters)
        {
          if (player && player->isAlive())
          {
            alive_players.push_back(player.get());
          }
        }
        
        if (!alive_players.empty())
        {
          // Select first player in list
          BattleMonster* target = alive_players[0];
          
          MonsterAction enemy_action;
          enemy_action.monster = attacker.get();
          enemy_action.target = target;
          enemy_action.action = ATTACK;  // Enemies only use basic attack
          
          startAttackAnimation(enemy_action);
        }
        else
        {
          // No valid target, skip this enemy
          current_executing_action++;
          attack_delay_timer = ATTACK_DELAY;
        }
      }
      else
      {
        // Monster is dead or stunned, skip
        current_executing_action++;
        attack_delay_timer = ATTACK_DELAY;
      }
      return;
    }
    
    // Apply poison damage at end of turn
    if (current_executing_action == (int)(player_actions.size() + enemy_monsters.size()))
    {
      for (auto& monster : player_monsters)
      {
        if (monster && monster->isPoisoned())
        {
          int poison_damage = monster->getMaxHP() / 10;
          monster->takeDamage(poison_damage);
        }
      }
      
      for (auto& monster : enemy_monsters)
      {
        if (monster && monster->isPoisoned())
        {
          int poison_damage = monster->getMaxHP() / 10;
          monster->takeDamage(poison_damage);
        }
      }
      current_executing_action++;
      attack_delay_timer = ATTACK_DELAY;
      return;
    }
    
    // Round complete, start next round
    if (current_executing_action > (int)(player_actions.size() + enemy_monsters.size()))
    {
      current_turn++;
      show_round_announce = true;
      round_announce_x = -500.0f;
      round_phase = FLYING_IN;
      round_announce_timer = 0.0f;
    }
  }
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
    show_quit_confirm = false;
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
    // Mark level as passed
    debug_log("current_level: %d\n", current_level);
    if (current_level > 0) {
      LevelMenu::getInstance()->markLevelPassed(current_level);
    }
  }
  else if (all_player_defeated && !player_monsters.empty())
  {
    state = BATTLE_LOST;
  }
}

void BattleField::draw()
{
  auto FC = FontCenter::get_instance();

  // Draw background image
  if (background_img) {
    al_draw_bitmap(background_img, 0, 0, 0);
  } else {
    // Fallback to colored background if image not loaded
    al_clear_to_color(al_map_rgb(100, 150, 200));
    al_draw_filled_rectangle(0, 500, FIELD_WIDTH, FIELD_HEIGHT,
                             al_map_rgb(100, 200, 100));
  }

  switch (state)
  {
  case BATTLE_ONGOING:
  case BATTLE_PAUSED:
    drawMonsters();
    drawUI();
    
    // Draw damage displays (floating numbers)
    drawDamageDisplays();
    
    // Draw ultimate videos (fullscreen overlay)
    drawUltimateVideos();
    
    // Draw round announcement animation
    if (show_round_announce)
    {
      drawRoundAnnounce();
    }
    
    // Draw action selection UI
    if (waiting_for_action_selection && round_phase == SELECTING_ACTIONS)
    {
      drawActionSelection();
    }
    break;

  case BATTLE_WON:
    drawMonsters();
    drawUI();
    
    // Draw damage displays (show final kill damage)
    drawDamageDisplays();
    
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
    
    // Draw damage displays (show final kill damage)
    drawDamageDisplays();
    
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
  monster_walk_frame_count++;
  monster_walk_frame_count %= 72;
  auto FC = FontCenter::get_instance();
  
  // Draw player monsters (left side)
  for (size_t i = 0; i < player_monsters.size(); ++i)
  {
    auto& battle_monster = player_monsters[i];
    if (battle_monster && battle_monster->getMonster() && battle_monster->getMonster()->getWalkFrame(monster_walk_frame_count/8))
    {
      // Use animated position if this monster is currently attacking
      int x, y;
      if (attack_anim_state != ANIM_NONE && attacking_monster == battle_monster.get())
      {
        x = attacker_current_x;
        y = attacker_current_y;
      }
      else
      {
        x = battle_monster->getPositionX();
        y = battle_monster->getPositionY();
      }
      
      al_draw_bitmap(battle_monster->getMonster()->getWalkFrame(monster_walk_frame_count/8), x, y, 0);
      
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
    if (battle_monster && battle_monster->getMonster() && battle_monster->getMonster()->getWalkFrame(monster_walk_frame_count/8))
    {
      // Use animated position if this monster is currently attacking
      int x, y;
      if (attack_anim_state != ANIM_NONE && attacking_monster == battle_monster.get())
      {
        x = attacker_current_x;
        y = attacker_current_y;
      }
      else
      {
        x = battle_monster->getPositionX();
        y = battle_monster->getPositionY();
      }
      
      al_draw_bitmap(battle_monster->getMonster()->getWalkFrame(monster_walk_frame_count/8), x, y, ALLEGRO_FLIP_HORIZONTAL);
      
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
  const int confirm_width = 550;
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

void BattleField::drawRoundAnnounce()
{
  FontCenter* FC = FontCenter::get_instance();
  
  // Draw semi-transparent background for better visibility
  al_draw_filled_rectangle(0, FIELD_HEIGHT / 2 - 80, FIELD_WIDTH, FIELD_HEIGHT / 2 + 80,
                          al_map_rgba(0, 0, 0, 100));
  
  // Draw "Round X" text
  if (FC->caviar_dreams[FontSize::LARGE])
  {
    char round_text[32];
    snprintf(round_text, sizeof(round_text), "Round %d", current_turn);
    
    // Draw shadow for depth
    al_draw_text(FC->caviar_dreams[FontSize::LARGE],
                al_map_rgb(0, 0, 0),
                round_announce_x + 3,
                FIELD_HEIGHT / 2 - 22,
                ALLEGRO_ALIGN_CENTER,
                round_text);
    
    // Draw main text
    al_draw_text(FC->caviar_dreams[FontSize::LARGE],
                al_map_rgb(255, 215, 0),  // Gold color
                round_announce_x,
                FIELD_HEIGHT / 2 - 25,
                ALLEGRO_ALIGN_CENTER,
                round_text);
  }
}

void BattleField::drawActionSelection()
{
  FontCenter* FC = FontCenter::get_instance();
  DataCenter* DC = DataCenter::get_instance();
  
  // Don't show UI during delay between selections
  if (action_selection_delay_timer > 0.0f)
    return;
  
  if (current_selecting_monster >= (int)player_monsters.size())
    return;
    
  auto& current_monster = player_monsters[current_selecting_monster];
  if (!current_monster || !current_monster->isAlive())
    return;
  
  // Draw semi-transparent overlay on top half only
  al_draw_filled_rectangle(0, 0, FIELD_WIDTH, FIELD_HEIGHT, al_map_rgba(0, 0, 0, 100));
  
  // Draw action bar image at bottom
  const int panel_height = 220;
  const int panel_y = FIELD_HEIGHT - panel_height;
  
  // Calculate which section the mouse is hovering over
  Point mouse(DC->mouse.x, DC->mouse.y);
  int hover_section = -1;  // -1: none, 0: attack, 1: ability, 2: ultimate
  
  if (mouse.y >= panel_y) {
    if (55 < mouse.x && mouse.x < 450) {
      hover_section = 0;  // Attack
    } else if (450 < mouse.x && mouse.x < 830) {
      hover_section = 1;  // Ability
    } else if (830 < mouse.x && mouse.x < 1225) {
      hover_section = 2;  // Ultimate
    }
  }
  
  // Play hover sound when entering a new section
  if (hover_section != -1 && hover_section != last_hover_section) {
    auto SC = SoundCenter::get_instance();
    SC->play("./assets/sound/battlefield/hover.mp3", ALLEGRO_PLAYMODE_ONCE);
    last_hover_section = hover_section;
  } else if (hover_section == -1) {
    last_hover_section = -1;  // Reset when not hovering
  }
  
  // Draw appropriate action bar based on hover
  ALLEGRO_BITMAP* current_actionbar = actionbar_img;
  if (hover_section == 0 && actionbar_attack_img) {
    current_actionbar = actionbar_attack_img;
  } else if (hover_section == 1 && actionbar_abality_img) {
    current_actionbar = actionbar_abality_img;
  } else if (hover_section == 2 && actionbar_ultimate_img) {
    current_actionbar = actionbar_ultimate_img;
  }
  
  if (current_actionbar) {
    al_draw_bitmap(current_actionbar, 0, panel_y, 0);
  } else {
    // Fallback if image not loaded
    al_draw_filled_rectangle(0, panel_y, FIELD_WIDTH, FIELD_HEIGHT, al_map_rgb(30, 30, 50));
    al_draw_rectangle(0, panel_y, FIELD_WIDTH, FIELD_HEIGHT, al_map_rgb(150, 150, 200), 3);
  }
  
  // Draw title at top of panel
  if (FC->caviar_dreams[FontSize::MEDIUM])
  {
    char title[64];
    snprintf(title, sizeof(title), "Monster %d - Select Action", current_selecting_monster + 1);
    al_draw_text(FC->caviar_dreams[FontSize::MEDIUM], al_map_rgb(255, 255, 255), FIELD_WIDTH / 2, panel_y + 15,
                ALLEGRO_ALIGN_CENTER,
                title);
  }
  
  // Note: Button click handling is done in updateBattle() for proper input processing
  // Hitboxes are calculated based on three equal parts of the action bar
}

void BattleField::startAttackAnimation(const MonsterAction& action)
{
  if (!action.monster || !action.monster->isAlive() || !action.target || !action.target->isAlive())
  {
    // Skip invalid action, move to next
    current_executing_action++;
    attack_delay_timer = 0.5f;
    return;
  }
  
  // Store action and initialize animation
  current_action = action;
  attacking_monster = action.monster;
  attack_target = action.target;
  
  // Save original position
  attacker_original_x = attacking_monster->getPositionX();
  attacker_original_y = attacking_monster->getPositionY();
  attacker_current_x = attacker_original_x;
  attacker_current_y = attacker_original_y;
  
  // Start movement animation
  attack_anim_state = ANIM_MOVING_TO_TARGET;
  attack_anim_timer = 0.0f;
}

void BattleField::updateAttackAnimation(float delta_time)
{
  const float MOVE_DURATION = 0.15f;  // Time to move to target (fast)
  const float ATTACK_DURATION = 0.2f; // Time spent attacking
  const float MOVE_BACK_DURATION = 0.15f; // Time to move back
  
  attack_anim_timer += delta_time;
  
  switch (attack_anim_state)
  {
    case ANIM_MOVING_TO_TARGET:
    {
      float progress = attack_anim_timer / MOVE_DURATION;
      if (progress >= 1.0f)
      {
        // Reached target position
        int target_x = attack_target->getPositionX();
        int target_y = attack_target->getPositionY();
        
        // Move towards target (stop before reaching)
        if (attacking_monster->isPlayerMonster())
        {
          attacker_current_x = target_x - 50;  // Stop 50 pixels before enemy
        }
        else
        {
          attacker_current_x = target_x + 50;  // Stop 50 pixels after player
        }
        attacker_current_y = target_y;
        
        // Check if this is an ultimate - if so, play video first
        if (current_action.action == ULTIMATE_SKILL)
        {
          // Start playing ultimate video
          if (attacking_monster)
          {
            attacking_monster->playUltimateVideo();
          }
          attack_anim_state = ANIM_PLAYING_VIDEO;
          attack_anim_timer = 0.0f;
        }
        else
        {
          // For normal attacks and abilities, proceed directly to attacking
          attack_anim_state = ANIM_ATTACKING;
          attack_anim_timer = 0.0f;
        }
      }
      else
      {
        // Interpolate position
        int target_x = attack_target->getPositionX();
        int target_y = attack_target->getPositionY();
        
        int attack_pos_x;
        if (attacking_monster->isPlayerMonster())
        {
          attack_pos_x = target_x - 50;
        }
        else
        {
          attack_pos_x = target_x + 50;
        }
        
        attacker_current_x = attacker_original_x + (int)((attack_pos_x - attacker_original_x) * progress);
        attacker_current_y = attacker_original_y + (int)((target_y - attacker_original_y) * progress);
      }
      break;
    }
    
    case ANIM_ATTACKING:
    {
      if (attack_anim_timer >= ATTACK_DURATION)
      {
        // Execute the actual attack/skill
        executeAction(current_action);
        
        // Check if battle ended after damage was dealt (to show final kill damage)
        checkBattleEnd();
        
        // Start moving back
        attack_anim_state = ANIM_MOVING_BACK;
        attack_anim_timer = 0.0f;
      }
      break;
    }
    
    case ANIM_PLAYING_VIDEO:
    {
      // Wait for ultimate video to finish playing
      if (attacking_monster && !attacking_monster->isVideoPlaying())
      {
        // Video finished, now execute damage and move back
        executeAction(current_action);
        
        // Check if battle ended after damage was dealt (to show final kill damage)
        checkBattleEnd();
        
        attack_anim_state = ANIM_MOVING_BACK;
        attack_anim_timer = 0.0f;
      }
      // Stay at target position while video plays
      break;
    }
    
    case ANIM_MOVING_BACK:
    {
      float progress = attack_anim_timer / MOVE_BACK_DURATION;
      if (progress >= 1.0f)
      {
        // Returned to original position, finish animation
        attacker_current_x = attacker_original_x;
        attacker_current_y = attacker_original_y;
        attack_anim_state = ANIM_NONE;
        attacking_monster = nullptr;
        attack_target = nullptr;
        
        // Move to next action
        current_executing_action++;
        attack_delay_timer = 0.5f;  // Delay before next monster attacks
      }
      else
      {
        // Interpolate back to original position
        int current_x = attacker_current_x;
        int current_y = attacker_current_y;
        attacker_current_x = current_x + (int)((attacker_original_x - current_x) * progress);
        attacker_current_y = current_y + (int)((attacker_original_y - current_y) * progress);
      }
      break;
    }
    
    default:
      break;
  }
}

void BattleField::executeAction(const MonsterAction& action)
{
  if (!action.monster || !action.monster->isAlive() || !action.target || !action.target->isAlive())
    return;
  
  switch (action.action)
  {
    case ATTACK:
    {
      // Normal attack: calculate damage
      int damage = std::max(1, action.monster->getAttack() - action.target->getDefense());
      
      // Create damage display before applying damage
      createDamageDisplay(damage, action.target->getPositionX(), action.target->getPositionY(), false);
      
      action.target->takeDamage(damage);
      break;
    }
    
    case ABILITY:
      executeMonsterSkill(action.monster, action.target);
      break;
      
    case ULTIMATE_SKILL:
      executeMonsterUltimate(action.monster, action.target);
      break;
  }
}

void BattleField::executeMonsterSkill(BattleMonster* attacker, BattleMonster* target)
{
  // TODO: Play special effect video/animation for ability skill
  // PLACEHOLDER: For now, just deal 1.5x damage
  int damage = std::max(1, (int)(attacker->getAttack() * 1.5f) - target->getDefense());
  
  // Create damage display for skill (critical style)
  createDamageDisplay(damage, target->getPositionX(), target->getPositionY(), true);
  
  target->takeDamage(damage);
  
  // TODO: Add particle effects, screen shake, skill-specific visuals
}

void BattleField::executeMonsterUltimate(BattleMonster* attacker, BattleMonster* target)
{
  // Deal 2x damage (video already played during animation)
  int damage = std::max(1, (attacker->getAttack() * 2) - target->getDefense());
  
  // Create damage display for ultimate (critical style, larger)
  createDamageDisplay(damage, target->getPositionX(), target->getPositionY(), true);
  
  target->takeDamage(damage);
}

void BattleField::drawUltimateVideos()
{
  // Check all monsters for playing videos and draw them as fullscreen overlays
  // Player monsters first
  for (auto& battle_monster : player_monsters)
  {
    if (battle_monster && battle_monster->isVideoPlaying())
    {
      ALLEGRO_BITMAP* frame = battle_monster->getVideoFrame();
      if (frame)
      {
        // Draw video frame scaled to screen size (or centered)
        int video_w = al_get_bitmap_width(frame);
        int video_h = al_get_bitmap_height(frame);
        
        // Scale to fit screen while maintaining aspect ratio
        float scale_x = (float)FIELD_WIDTH / video_w;
        float scale_y = (float)FIELD_HEIGHT / video_h;
        float scale = std::min(scale_x, scale_y);
        
        int draw_w = (int)(video_w * scale);
        int draw_h = (int)(video_h * scale);
        int draw_x = (FIELD_WIDTH - draw_w) / 2;
        int draw_y = (FIELD_HEIGHT - draw_h) / 2;
        
        // Draw semi-transparent black background
        al_draw_filled_rectangle(0, 0, FIELD_WIDTH, FIELD_HEIGHT,
                                al_map_rgba(0, 0, 0, 200));
        
        // Draw scaled video frame
        al_draw_scaled_bitmap(frame,
                             0, 0, video_w, video_h,
                             draw_x, draw_y, draw_w, draw_h,
                             0);
        return; // Only show one video at a time
      }
    }
  }
  
  // Enemy monsters
  for (auto& battle_monster : enemy_monsters)
  {
    if (battle_monster && battle_monster->isVideoPlaying())
    {
      ALLEGRO_BITMAP* frame = battle_monster->getVideoFrame();
      if (frame)
      {
        int video_w = al_get_bitmap_width(frame);
        int video_h = al_get_bitmap_height(frame);
        
        float scale_x = (float)FIELD_WIDTH / video_w;
        float scale_y = (float)FIELD_HEIGHT / video_h;
        float scale = std::min(scale_x, scale_y);
        
        int draw_w = (int)(video_w * scale);
        int draw_h = (int)(video_h * scale);
        int draw_x = (FIELD_WIDTH - draw_w) / 2;
        int draw_y = (FIELD_HEIGHT - draw_h) / 2;
        
        al_draw_filled_rectangle(0, 0, FIELD_WIDTH, FIELD_HEIGHT,
                                al_map_rgba(0, 0, 0, 200));
        
        al_draw_scaled_bitmap(frame,
                             0, 0, video_w, video_h,
                             draw_x, draw_y, draw_w, draw_h,
                             0);
        return;
      }
    }
  }
}

void BattleField::createDamageDisplay(int damage, int target_x, int target_y, bool is_critical)
{
  DamageDisplay display;
  display.damage = damage;
  display.x = target_x + 40;  // Offset to center above monster
  display.y = target_y - 20;  // Start slightly above monster
  display.timer = 0.0f;
  display.alpha = 1.0f;
  display.is_critical = is_critical;
  damage_displays.push_back(display);
}

void BattleField::updateDamageDisplays(float delta_time)
{
  const float DISPLAY_DURATION = 1.5f;  // 1.5 seconds total display time
  const float FLOAT_SPEED = 30.0f;      // Pixels per second upward movement
  
  // Update all active damage displays
  for (auto& display : damage_displays)
  {
    display.timer += delta_time;
    display.y -= FLOAT_SPEED * delta_time;  // Float upward
    
    // Fade out in the last half of the duration
    if (display.timer > DISPLAY_DURATION / 2)
    {
      float fade_progress = (display.timer - DISPLAY_DURATION / 2) / (DISPLAY_DURATION / 2);
      display.alpha = 1.0f - fade_progress;
    }
  }
  
  // Remove expired displays
  damage_displays.erase(
    std::remove_if(damage_displays.begin(), damage_displays.end(),
      [DISPLAY_DURATION](const DamageDisplay& d) { return d.timer >= DISPLAY_DURATION; }),
    damage_displays.end()
  );
}

void BattleField::drawDamageDisplays()
{
  auto FC = FontCenter::get_instance();
  if (!FC->caviar_dreams[FontSize::LARGE]) return;
  
  for (const auto& display : damage_displays)
  {
    // Calculate alpha for fading
    int alpha = (int)(display.alpha * 255);
    
    // Choose color and size based on type
    ALLEGRO_COLOR color;
    ALLEGRO_FONT* font;
    
    if (display.is_critical)
    {
      // Critical hits (abilities/ultimates): larger, orange/red
      color = al_map_rgba(255, 150, 0, alpha);
      font = FC->caviar_dreams[FontSize::LARGE];
    }
    else
    {
      // Normal attacks: white
      color = al_map_rgba(255, 255, 255, alpha);
      font = FC->caviar_dreams[FontSize::MEDIUM];
    }
    
    // Draw damage number
    char damage_text[16];
    snprintf(damage_text, sizeof(damage_text), "%d", display.damage);
    
    // Draw shadow for better visibility
    al_draw_text(font,
                al_map_rgba(0, 0, 0, alpha / 2),
                display.x + 2, display.y + 2,
                ALLEGRO_ALIGN_CENTER,
                damage_text);
    
    // Draw main damage number
    al_draw_text(font,
                color,
                display.x, display.y,
                ALLEGRO_ALIGN_CENTER,
                damage_text);
  }
}

void BattleField::end()
{
}
