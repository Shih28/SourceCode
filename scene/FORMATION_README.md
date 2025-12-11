# Formation Scene Documentation

## Overview
The `Formation` scene allows players to select up to 5 monsters for battle before entering the battlefield. It provides an intuitive interface for team composition with visual feedback.

## Features

### Monster Selection (Up to 5 monsters)
- **5 Empty Slots**: Initially displayed horizontally across the screen
- **Add Button (+)**: Click the + button in any empty slot to open monster selection
- **Monster Popup**: Shows all available monsters from player's collection in a grid layout
- **Visual Feedback**: 
  - Hovering highlights monsters
  - Already-selected monsters show "USED" indicator
  - Cannot select the same monster twice

### Monster Management
- **Add Monster**: Click + button → Select from popup
- **Remove Monster**: Right-click on filled slot to remove monster
- **Visual Display**: 
  - Monster image shown in slot
  - Species name and color-coded text (Water=Blue, Fire=Red, Wind=Green, Lightning=Yellow)
  - Slot numbers (Slot 1-5)

### Navigation
- **GO Button**: Appears at bottom when at least 1 monster is selected
  - Transitions to BATTLE state
  - Automatically sets up BattleField with selected monsters
  - Uses same style as LevelMenu's GO button
- **ESC Key**: Returns to Level Menu
- **Cancel Button**: In popup, closes selection without choosing

## User Flow

```
LevelMenu (select level) 
    ↓ Click GO
Formation (select team)
    ↓ Click GO (with 1-5 monsters selected)
BattleField (battle starts)
```

## UI Layout

```
┌─────────────────────────────────────────────┐
│           Select Your Team                  │
│   Choose up to 5 monsters - Right click to │
│                  remove                     │
│                                            │
│  ┌───┐  ┌───┐  ┌───┐  ┌───┐  ┌───┐       │
│  │ + │  │ + │  │ + │  │ + │  │ + │       │
│  └───┘  └───┘  └───┘  └───┘  └───┘       │
│  Slot1  Slot2  Slot3  Slot4  Slot5        │
│                                            │
│                                            │
│          X Monsters Selected               │
│              ┌────────┐                    │
│              │   GO   │                    │
│              └────────┘                    │
└─────────────────────────────────────────────┘
```

### Monster Selection Popup

```
┌─────────────────────────────────────────────┐
│          Choose a Monster                   │
│                                            │
│  ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐│
│  │ M │ │ M │ │ M │ │ M │ │ M │ │ M │ │ M ││
│  └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘│
│  ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐│
│  │ M │ │ M │ │ M │ │ M │ │ M │ │ M │ │ M ││
│  └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘│
│                                            │
│              ┌──────────┐                  │
│              │  Cancel  │                  │
│              └──────────┘                  │
└─────────────────────────────────────────────┘
```

## Integration with Game States

### Game.h STATE enum
Formation uses `Game::STATE::FORMATION`

### State Transitions
- **LEVEL → FORMATION**: When GO is clicked in LevelMenu
- **FORMATION → BATTLE**: When GO is clicked in Formation with monsters
- **FORMATION → LEVEL**: When ESC is pressed

### Game.cpp Integration
Added to all three switch statements:
1. `game_update()` - Handles Formation update logic
2. `game_draw()` - Renders Formation scene
3. `scene_init()` - Initializes Formation scene

## Code Structure

### Key Methods

#### `init()`
- Clears selected monsters
- Loads UI images
- Sets up slot positions and button rectangles

#### `update()`
- Handles two modes: normal selection and popup mode
- **Popup Mode**:
  - Detects clicks on monsters in grid
  - Handles cancel button
- **Normal Mode**:
  - Detects add button clicks for empty slots
  - Detects right-clicks for removal
  - Handles GO button (requires ≥1 monster)
  - Handles ESC key

#### `draw()`
- Draws background and title
- Calls `drawMonsterSlots()` for the 5 slots
- Calls `drawMonsterSelectionPopup()` if popup open
- Calls `drawGoButton()` with count display

### Helper Functions

- `openMonsterSelection(slot)` - Opens popup for specific slot
- `closeMonsterSelection()` - Closes popup
- `selectMonster(monster)` - Assigns monster to selected slot
- `removeMonster(slot)` - Clears a slot
- `getSelectedMonsterCount()` - Returns count of filled slots

## Visual Design

### Colors
- **Background**: Dark blue (50, 50, 100)
- **Slots**: Purple-gray (80, 80, 120)
- **Add Button**: Green (70, 150, 70) → Brighter on hover (100, 200, 100)
- **GO Button**: Uses images or green fallback
- **Disabled GO**: Gray (100, 100, 100)
- **Popup Overlay**: Black with 70% opacity
- **Popup Background**: Dark blue (40, 40, 80)

### Species Colors
- **Water**: Light blue (100, 150, 255)
- **Fire**: Orange-red (255, 100, 50)
- **Wind**: Light green (150, 255, 150)
- **Lightning**: Yellow (255, 255, 100)

## Monster Selection Logic

```cpp
// Example: How monsters are passed to battle
std::vector<Monster*> battle_monsters;
for (int i = 0; i < 5; ++i) {
    if (selected_monsters[i] != nullptr) {
        battle_monsters.push_back(selected_monsters[i]);
    }
}
BattleField::get()->setPlayerMonsters(battle_monsters);
```

## Assets Used

- `./assets/image/scene/start.png` - Background (fallback)
- `./assets/image/levelmenu/go.png` - GO button
- `./assets/image/levelmenu/go_hover.png` - GO button hover state

## Future Enhancements

1. **Monster Stats Preview**: Show HP, ATK, DEF when hovering
2. **Formation Presets**: Save/load favorite team compositions
3. **Drag & Drop**: Reorder monsters by dragging
4. **Animation**: Slot fill animation when monster selected
5. **Sound Effects**: Click sounds for selection/removal
6. **Monster Sorting**: Sort by level, species, etc.
7. **Search/Filter**: Filter monsters by species or type
8. **Team Synergy Display**: Show type effectiveness composition
