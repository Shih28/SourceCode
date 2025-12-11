# BattleMonster Usage Guide

## Overview
The `BattleMonster` class wraps the `Monster` class to add battle-specific data that should not be permanently stored with the monster. This keeps the `Monster` class clean and separates persistent data from temporary battle state.

## Architecture

```
Player owns: vector<Monster>
            ↓
BattleField wraps: vector<unique_ptr<BattleMonster>>
                  ↓
Each BattleMonster contains:
    - Monster* (reference to original)
    - current_hp (temporary)
    - current_target (temporary)
    - status_effects (temporary)
    - position (temporary)
```

## Key Features

### 1. Battle Stats
- **HP Management**: Max HP and current HP (calculated from monster level/species)
- **Combat Stats**: Attack, Defense, Speed
- **Alive/Dead Status**: Automatic HP-based checks

```cpp
BattleMonster* bm = player_monsters[0].get();
bm->takeDamage(25);  // Reduce HP by 25
bm->heal(10);        // Restore HP by 10
bool alive = bm->isAlive();  // Check if HP > 0
```

### 2. Status Effects
- **Stun**: Prevents actions for N turns
- **Poison**: Deals damage each turn for N turns

```cpp
bm->setStun(2);      // Stun for 2 turns
bm->setPoison(3);    // Poison for 3 turns
bool stunned = bm->isStunned();
bool poisoned = bm->isPoisoned();

// Call each turn to decrement counters and apply poison damage
bm->decrementStatusEffects();
```

### 3. Targeting System
```cpp
BattleMonster* target = enemy_monsters[0].get();
player_monsters[0]->setCurrentTarget(target);
```

### 4. Position Tracking
```cpp
bm->setPosition(100, 200);
int x = bm->getPositionX();
int y = bm->getPositionY();
```

### 5. Turn Management
```cpp
if (!bm->hasActedThisTurn()) {
    // Perform action
    bm->setActedThisTurn(true);
}

// At start of new turn
bm->resetTurnState();
```

## Example: Implementing Attack Logic

```cpp
void BattleField::performAttack(BattleMonster* attacker, BattleMonster* target) {
    if (attacker->isStunned()) {
        // Can't attack while stunned
        return;
    }
    
    // Calculate damage
    int damage = attacker->getAttack() - target->getDefense();
    damage = (damage < 1) ? 1 : damage; // Minimum 1 damage
    
    // Apply type effectiveness (if implemented)
    // damage = applyTypeModifier(attacker->getMonster()->getSpecies(), 
    //                            target->getMonster()->getSpecies(), damage);
    
    // Deal damage
    target->takeDamage(damage);
    
    // Mark as acted
    attacker->setActedThisTurn(true);
}
```

## Example: Turn-Based Battle Loop

```cpp
void BattleField::processTurn() {
    // Reset all monsters' turn state
    for (auto& bm : player_monsters) {
        bm->resetTurnState();
        bm->decrementStatusEffects();
    }
    for (auto& bm : enemy_monsters) {
        bm->resetTurnState();
        bm->decrementStatusEffects();
    }
    
    // Create priority queue based on speed
    std::vector<BattleMonster*> turn_order;
    for (auto& bm : player_monsters) {
        if (bm->isAlive()) turn_order.push_back(bm.get());
    }
    for (auto& bm : enemy_monsters) {
        if (bm->isAlive()) turn_order.push_back(bm.get());
    }
    
    // Sort by speed (fastest first)
    std::sort(turn_order.begin(), turn_order.end(), 
        [](BattleMonster* a, BattleMonster* b) {
            return a->getSpeed() > b->getSpeed();
        });
    
    // Process each monster's turn
    for (auto* bm : turn_order) {
        if (bm->isStunned()) continue;
        
        if (bm->isPlayerMonster()) {
            // Wait for player input
            handlePlayerAction(bm);
        } else {
            // AI decision
            handleEnemyAI(bm);
        }
    }
    
    current_turn++;
}
```

## Stat Calculation

The `calculateStats()` method computes battle stats based on:
1. **Species bonuses**:
   - Water: +20 HP, +5 Defense
   - Fire: +10 Attack, +5 Speed
   - Wind: +15 Speed, -5 Defense
   - Lightning: +5 Attack, +10 Speed

2. **Level scaling** (placeholder - needs Monster::getLevel()):
   - HP: base + (level * 10)
   - Attack: base + (level * 2)
   - Defense: base + (level * 1)
   - Speed: base + (level * 1)

## Visual Feedback

The `drawMonsters()` method now displays:
- **HP Bars**: Color-coded by HP percentage
  - Green: > 50% HP
  - Yellow: 25-50% HP
  - Red: < 25% HP
- **HP Text**: Shows current/max HP
- **Status Icons**: Shows STUN and POISON effects

## TODO Enhancements

1. **Add more status effects**: Burn, Freeze, Sleep, Confusion
2. **Implement type effectiveness chart**
3. **Add critical hit system**
4. **Add dodge/accuracy mechanics**
5. **Add buffs/debuffs** (ATK up, DEF down, etc.)
6. **Add energy/mana system for special abilities**
7. **Add combo/chain attack system**

## Memory Management

- BattleField uses `std::unique_ptr<BattleMonster>` for automatic cleanup
- BattleMonster only stores a pointer to Monster (doesn't own it)
- When BattleField ends, all BattleMonsters are automatically destroyed
- Original Monster objects remain unchanged in Player's collection
