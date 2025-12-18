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
    enum BATTLE_STATUS {
        BATTLE_INIT,      // Initialize battle
        BATTLE_ONGOING,   // Battle in progress
        BATTLE_PAUSED,    // Battle paused
        BATTLE_WON,       // Player won
        BATTLE_LOST       // Player lost
    };

    void init() override;
    void scene_init();
    void update() override;
    void draw() override;
    void end() override;

    static BattleField* get() {
        static BattleField BF;
        return &BF;
    }

    // Battle management
    void setPlayerMonsters(std::vector<Monster*> monsters);
    void setLevel(int level);
    void setState(BATTLE_STATUS s) { state = s; }
    BATTLE_STATUS getState() { return state; }

private:
    BattleField() : state(BATTLE_INIT) {}
    
    BATTLE_STATUS state;
    
    // Battle monster lists (wrappers around Monster with battle data)
    std::vector<Monster *> level_monsters[6];
    std::vector<std::unique_ptr<BattleMonster>> player_monsters;
    std::vector<std::unique_ptr<BattleMonster>> enemy_monsters;
    
    // Battle variables
    int current_turn;
    float battle_timer;  // Timer for turn progression
    int current_level;  // Track which level is being played
    
    // Action selection
    enum ACTION_TYPE { ATTACK, ABILITY, ULTIMATE_SKILL };
    struct MonsterAction {
        BattleMonster* monster;
        ACTION_TYPE action;
        BattleMonster* target;
    };
    std::vector<MonsterAction> player_actions;
    int current_selecting_monster;  // Index of monster currently selecting action
    bool waiting_for_action_selection;
    BattleMonster* selected_target;
    float action_selection_delay_timer;  // Delay before showing next selection

    // Monster animation variables
    int monster_walk_frame_count;
    
    // Round announcement animation
    bool show_round_announce;
    float round_announce_x;
    float round_announce_timer;
    enum ROUND_PHASE { FLYING_IN, PAUSED, FLYING_OUT, SELECTING_ACTIONS, EXECUTING_ACTIONS };
    ROUND_PHASE round_phase;
    float attack_delay_timer;  // Delay between attacks
    int current_executing_action;  // Index of action being executed
    
    // Attack animation variables
    enum ATTACK_ANIMATION_STATE { ANIM_NONE, ANIM_MOVING_TO_TARGET, ANIM_ATTACKING, ANIM_PLAYING_VIDEO, ANIM_MOVING_BACK };
    ATTACK_ANIMATION_STATE attack_anim_state;
    BattleMonster* attacking_monster;
    BattleMonster* attack_target;
    int attacker_original_x;
    int attacker_original_y;
    int attacker_current_x;
    int attacker_current_y;
    float attack_anim_timer;
    MonsterAction current_action;
    
    // Damage display
    struct DamageDisplay {
        int damage;
        float x;
        float y;
        float timer;
        float alpha;
        bool is_critical;
    };
    std::vector<DamageDisplay> damage_displays;
    
    // Pause animation variables
    bool pause_animating;
    double pause_current_y;
    double pause_velocity;
    double pause_target_y;
    bool show_quit_confirm;
    
    // Images
    ALLEGRO_BITMAP* background_img;
    ALLEGRO_BITMAP* actionbar_img;
    ALLEGRO_BITMAP* actionbar_attack_img;
    ALLEGRO_BITMAP* actionbar_abality_img;
    ALLEGRO_BITMAP* actionbar_ultimate_img;
    
    // Sounds
    int last_hover_section;  // Track last hovered section to avoid repeating sound
    
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
    void drawRoundAnnounce();
    void drawActionSelection();
    void drawUltimateVideos();
    void executeAction(const MonsterAction& action);
    void executeMonsterSkill(BattleMonster* attacker, BattleMonster* target);
    void executeMonsterUltimate(BattleMonster* attacker, BattleMonster* target);
    void startAttackAnimation(const MonsterAction& action);
    void updateAttackAnimation(float delta_time);
    void createDamageDisplay(int damage, int target_x, int target_y, bool is_critical = false);
    void updateDamageDisplays(float delta_time);
    void drawDamageDisplays();
};

#endif
