#ifndef BATTLEFIELD_H_INCLUDED
#define BATTLEFIELD_H_INCLUDED

#include "Scene.h"
#include "../Monster.h"
#include "BattleMonster.h"
#include <vector>
#include <memory>

/**
 * @brief Battle scene where player monsters fight against enemies
 */
class BattleField : public Scene {
public:
    /**
     * @brief Battle states
     */
    enum STATE_B {
        BATTLE_INIT,      // Initialize battle
        BATTLE_ONGOING,   // Battle in progress
        BATTLE_PAUSED,    // Battle paused
        BATTLE_WON,       // Player won
        BATTLE_LOST       // Player lost
    };

    void init() override;
    void update() override;
    void draw() override;
    void end() override;

    static BattleField* get() {
        static BattleField BF;
        return &BF;
    }

    // Battle management
    void setPlayerMonsters(std::vector<Monster*> monsters);
    void setEnemyMonsters(std::vector<Monster*> monsters);
    void setState(STATE_B s) { state = s; }
    STATE_B getState() { return state; }

private:
    BattleField() : state(BATTLE_INIT) {}
    
    STATE_B state;
    
    // Battle monster lists (wrappers around Monster with battle data)
    std::vector<std::unique_ptr<BattleMonster>> player_monsters;
    std::vector<std::unique_ptr<BattleMonster>> enemy_monsters;
    
    // Battle variables
    int current_turn;
    float battle_timer;
    
    // Pause animation variables
    bool pause_animating;
    double pause_current_y;
    double pause_velocity;
    double pause_target_y;
    bool show_quit_confirm;
    
    // UI positions
    const int FIELD_WIDTH = 1280;
    const int FIELD_HEIGHT = 720;
    const int PLAYER_ZONE_X = 150;
    const int ENEMY_ZONE_X = 900;
    const int MONSTER_Y_START = 250;
    
    // Helper functions
    void updateBattle();
    void updatePause();
    void checkBattleEnd();
    void drawMonsters();
    void drawUI();
    void drawPauseBox();
    void drawQuitConfirm();
};

#endif
