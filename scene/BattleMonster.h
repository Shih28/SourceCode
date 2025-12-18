#ifndef BATTLEMONSTER_H_INCLUDED
#define BATTLEMONSTER_H_INCLUDED

#include "../Monster.h"
#include <allegro5/allegro_video.h>
#include <string>

/**
 * @brief Wrapper class for Monster that adds battle-specific data
 * @details This class stores temporary battle state without modifying the original Monster
 */
class BattleMonster {
public:
    /**
     * @brief Constructor that wraps a Monster for battle
     * @param m Pointer to the Monster to wrap
     * @param is_player True if this is a player's monster, false if enemy
     */
    BattleMonster(Monster* m, bool is_player = true);
    
    /**
     * @brief Destructor to clean up video resources
     */
    ~BattleMonster();
    
    // Monster reference
    Monster* getMonster() { return monster; }
    const Monster* getMonster() const { return monster; }
    
    // Battle stats
    int getCurrentHP() const { return current_hp; }
    int getMaxHP() const { return max_hp; }
    void setCurrentHP(int hp) { current_hp = (hp < 0) ? 0 : (hp > max_hp ? max_hp : hp); }
    void takeDamage(int damage) { current_hp = (current_hp - damage < 0) ? 0 : current_hp - damage; }
    void heal(int amount) { current_hp = (current_hp + amount > max_hp) ? max_hp : current_hp + amount; }
    bool isAlive() const { return current_hp > 0; }
    bool isDead() const { return current_hp <= 0; }
    
    // Combat stats
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    
    // Battle state
    BattleMonster* getCurrentTarget() { return current_target; }
    void setCurrentTarget(BattleMonster* target) { current_target = target; }
    bool isPlayerMonster() const { return is_player; }
    
    // Status effects (can be expanded)
    bool isStunned() const { return stun_turns > 0; }
    bool isPoisoned() const { return poison_turns > 0; }
    int getStunTurns() const { return stun_turns; }
    int getPoisonTurns() const { return poison_turns; }
    void setStun(int turns) { stun_turns = turns; }
    void setPoison(int turns) { poison_turns = turns; }
    void decrementStatusEffects();
    
    // Position on battlefield
    int getPositionX() const { return position_x; }
    int getPositionY() const { return position_y; }
    void setPosition(int x, int y) { position_x = x; position_y = y; }
    
    // Action tracking
    bool hasActedThisTurn() const { return acted_this_turn; }
    void setActedThisTurn(bool acted) { acted_this_turn = acted; }
    void resetTurnState() { acted_this_turn = false; }
    
    // Ultimate video playback
    /**
     * @brief Play the ultimate animation video for this monster
     * @details Starts playing the video based on monster species
     */
    void playUltimateVideo();
    
    /**
     * @brief Update video playback state
     * @details Should be called every frame to update video
     */
    void updateVideo();
    
    /**
     * @brief Check if the ultimate video is currently playing
     * @return True if video is playing, false otherwise
     */
    bool isVideoPlaying() const { return video_playing; }
    
    /**
     * @brief Get the current video frame to draw
     * @return ALLEGRO_BITMAP pointer to current frame, or nullptr if not playing
     */
    ALLEGRO_BITMAP* getVideoFrame();
    
    /**
     * @brief Stop the video playback
     */
    void stopVideo();
    
private:
    Monster* monster; // Reference to the original monster
    bool is_player;   // Whether this is a player's monster or enemy
    
    // Battle stats (calculated from monster level/species)
    int max_hp;
    int current_hp;
    int attack;
    int defense;
    
    // Battle state
    BattleMonster* current_target;
    bool acted_this_turn;
    
    // Status effects
    int stun_turns;
    int poison_turns;
    
    // Position
    int position_x;
    int position_y;
    
    // Ultimate video
    ALLEGRO_VIDEO* ultimate_video;
    bool video_playing;
    std::string video_path;
    
    // Helper methods
    void calculateStats();
    void loadUltimateVideo();
    std::string getSpeciesString() const;
};

#endif
