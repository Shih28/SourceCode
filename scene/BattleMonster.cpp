#include "BattleMonster.h"
#include <allegro5/allegro.h>
#include "../Utils.h"

BattleMonster::BattleMonster(Monster* m, bool is_player)
    : monster(m), 
      is_player(is_player),
      current_target(nullptr),
      acted_this_turn(false),
      stun_turns(0),
      poison_turns(0),
      ability_used(false),
      ultimate_used(false),
      position_x(0),
      position_y(0),
      ultimate_video(nullptr),
      video_playing(false),
      video_path("")
{
    calculateStats();
    current_hp = max_hp; // Start at full HP
    loadUltimateVideo();
}

BattleMonster::~BattleMonster()
{
    if (ultimate_video) {
        al_close_video(ultimate_video);
        ultimate_video = nullptr;
    }
}

void BattleMonster::calculateStats()
{
    if (!monster) {
        max_hp = 100;
        attack = 10;
        defense = 10;
        return;
    }
    
    int base_hp = 100;
    int base_attack = 15;
    int base_defense = 10;
    
    // Species modifiers
    switch (monster->getSpecies()) {
        case Monster::WATER:
            base_hp += 20;
            base_defense += 5;
            break;
        case Monster::FIRE:
            base_attack += 15;
            break;
        case Monster::WIND:
            base_hp += 30;
            base_defense -= 10;
            break;
        case Monster::LIGHTNING:
            base_attack += 5;
            break;
        default:
            break;
    }
    
    int level = monster->getLevel();

    max_hp = base_hp + (level * 30);
    attack = base_attack + (level * 15);
    defense = base_defense + (level * 5);
}

void BattleMonster::decrementStatusEffects()
{
    if (stun_turns > 0) {
        stun_turns--;
    }
    
    if (poison_turns > 0) {
        poison_turns--;
        // Apply poison damage
        int poison_damage = max_hp / 10; // 10% of max HP
        takeDamage(poison_damage);
    }
}

void BattleMonster::loadUltimateVideo()
{
    if (!monster) {
        return;
    }
    
    std::string type_str = getTypeString();
    video_path = "./assets/video/monster/" + type_str + ".ogv";
    
    ultimate_video = al_open_video(video_path.c_str());
    if (!ultimate_video) {
        debug_log("<BattleMonster> failed to load ultimate video: %s\n", video_path.c_str());
        video_path = "";
        return;
    }
}

std::string BattleMonster::getTypeString() const
{
    if (!monster) {
        return "default";
    }
    
    switch (monster->getType()) {
        case Monster::BAD_GYAUMAL_BABY:
            return "BAD_GYAUMAL_BABY";
        case Monster::BAD_GYAUMAL_ADAULT:
            return "BAD_GYAUMAL_ADAULT";
        case Monster::FREETTLE_BABY:
            return "FREETTLE_BABY";
        case Monster::FREETTLE_ADAULT:
            return "FREETTLE_ADAULT";
        case Monster::DAKUABENJA_BABY:
            return "DAKUABENJA_BABY";
        case Monster::DAKUABENJA_ADAULT:
            return "DAKUABENJA_ADAULT";
        case Monster::LORD_OF_ATL_BABY:
            return "LORD_OF_ATL_BABY";
        case Monster::LORD_OF_ATL_ADAULT:
            return "LORD_OF_ATL_ADAULT";
        case Monster::VIRELIA_BABY:
            return "VIRELIA_BABY";
        case Monster::VIRELIA_ADAULT:
            return "VIRELIA_ADAULT";
        case Monster::PANDALF_BABY:
            return "PANDALF_BABY";
        case Monster::PANDALF_ADAULT:
            return "PANDALF_ADAULT";
        case Monster::THORDER_BABY:
            return "THORDER_BABY";
        case Monster::THORDER_ADAULT:
            return "THORDER_ADAULT";
        case Monster::FENNEVYR_BABY:
            return "FENNEVYR_BABY";
        case Monster::FENNEVYR_ADAULT:
            return "FENNEVYR_ADAULT";
        default:
            return "default";
    }
}

void BattleMonster::playUltimateVideo()
{
    if (!ultimate_video) {
        return;
    }
    
    // Start video with default mixer (audio system properly initialized)
    al_start_video(ultimate_video, al_get_default_mixer());
    al_seek_video(ultimate_video, 0.0);
    al_set_video_playing(ultimate_video, true);
    video_playing = true;
}

void BattleMonster::updateVideo()
{
    if (!ultimate_video) {
        return;
    }
    
    // Update playing state if video has finished
    if (video_playing && !al_is_video_playing(ultimate_video)) {
        video_playing = false;
    }
}

ALLEGRO_BITMAP* BattleMonster::getVideoFrame()
{
    if (!ultimate_video) {
        return nullptr;
    }
      
    return al_get_video_frame(ultimate_video);
}