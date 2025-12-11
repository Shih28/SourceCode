# BattleField Scene Documentation

## Overview
The `BattleField` class implements a base battle scene where player monsters fight against enemy monsters. It follows the project's scene architecture pattern and integrates with the existing game state system.

## Files Created
- `scene/BattleField.h` - Header file with class declaration
- `scene/BattleField.cpp` - Implementation file with game logic

## Features Implemented

### Battle States
- **BATTLE_INIT**: Initialization phase
- **BATTLE_ONGOING**: Active battle state
- **BATTLE_PAUSED**: Paused state
- **BATTLE_WON**: Victory state
- **BATTLE_LOST**: Defeat state

### Core Functionality
1. **Monster Management**
   - Support for up to 3 player monsters
   - Enemy monster management
   - Monster positioning (player on left, enemies on right)

2. **Battle Flow**
   - Turn-based battle system (structure ready)
   - Battle timer tracking
   - Victory/defeat condition checking
   - Pause functionality (ESC to resume, Q to quit)

3. **Visual Elements**
   - Sky blue background with green ground
   - Monster sprites displayed on battlefield
   - Pause button (top right corner)
   - Turn counter display
   - Victory/defeat messages
   - Pause overlay with semi-transparent background

### Controls
- **Mouse Click** (top right pause button): Pause battle
- **ESC Key** (when paused): Resume battle
- **Q Key** (when paused): Return to menu
- **ENTER Key** (victory): Continue to level menu
- **ENTER Key** (defeat): Return to main menu

## Integration with Game System

### Game.cpp Changes
- Added `#include "scene/BattleField.h"`
- Added `STATE::BATTLE` case in `game_update()` function
- Added `STATE::BATTLE` case in `game_draw()` function
- Added `STATE::BATTLE` case in `scene_init()` function

### Scene Pattern
Follows the same singleton pattern as other scenes:
```cpp
BattleField* bf = BattleField::get();
bf->init();
bf->update();
bf->draw();
bf->end();
```

## TODO / Future Enhancements

### High Priority
1. **Battle Logic**
   - Implement turn-based combat system
   - Add attack/defend actions
   - Implement damage calculation
   - Add monster HP tracking

2. **Enemy System**
   - Load enemy monsters based on level/difficulty
   - AI behavior for enemy actions

3. **Visual Assets**
   - Load proper battle background images
   - Add battle animations
   - Implement HP bars
   - Add skill/action buttons UI

### Medium Priority
4. **Battle Mechanics**
   - Status effects (poison, stun, etc.)
   - Special skills/abilities
   - Type effectiveness (Fire vs Water, etc.)
   - Experience/rewards system

5. **UI Improvements**
   - Action menu for player turns
   - Monster info panels
   - Skill selection interface
   - Battle log/message system

### Low Priority
6. **Polish**
   - Sound effects for actions
   - Battle music
   - Victory/defeat animations
   - Particle effects

## Usage Example

To transition to the battle scene from another scene (e.g., from LevelMenu):
```cpp
// In some scene update function
Player* pl = Player::getPlayer();
pl->setrequest(Game::STATE::BATTLE);
```

The BattleField will automatically:
1. Initialize with player's monsters
2. Set up enemy monsters (when implemented)
3. Start the battle in BATTLE_ONGOING state

## Notes
- The scene automatically loads up to 3 monsters from the player's collection
- Battle end detection is currently placeholder - needs HP system implementation
- All drawing uses Allegro5 primitives and the project's existing font/image systems
- Scene follows the project's memory management patterns (singleton, no manual cleanup needed)
