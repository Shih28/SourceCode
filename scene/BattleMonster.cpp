#include "BattleMonster.h"

BattleMonster::BattleMonster(Monster* m, bool is_player)
    : monster(m), 
      is_player(is_player),
      current_target(nullptr),
      acted_this_turn(false),
      stun_turns(0),
      poison_turns(0),
      position_x(0),
      position_y(0)
{
    calculateStats();
    current_hp = max_hp; // Start at full HP
}

void BattleMonster::calculateStats()
{
    if (!monster) {
        max_hp = 100;
        attack = 10;
        defense = 10;
        speed = 10;
        return;
    }
    
    // TODO: Calculate based on monster's actual stats
    // For now, use placeholder calculations based on species
    // These should be adjusted based on your game's balance
    
    int base_hp = 100;
    int base_attack = 15;
    int base_defense = 10;
    int base_speed = 10;
    
    // Species modifiers
    switch (monster->getSpecies()) {
        case Monster::WATER:
            base_hp += 20;
            base_defense += 5;
            break;
        case Monster::FIRE:
            base_attack += 10;
            base_speed += 5;
            break;
        case Monster::WIND:
            base_speed += 15;
            base_defense -= 5;
            break;
        case Monster::LIGHTNING:
            base_attack += 5;
            base_speed += 10;
            break;
        default:
            break;
    }
    
    // TODO: Level scaling - should use monster->getLevel() if available
    // For now, assume level 1
    int level = 1; // Replace with actual level when Monster has getLevel()
    
    max_hp = base_hp + (level * 10);
    attack = base_attack + (level * 2);
    defense = base_defense + (level * 1);
    speed = base_speed + (level * 1);
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
