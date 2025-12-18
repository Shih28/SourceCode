#ifndef SURVIVAL_H_INCLUDED
#define SURVIVAL_H_INCLUDED

#include "Scene.h"
#include "../shapes/Point.h"
#include <vector>
#include <string>

// Forward declaration
class ALLEGRO_BITMAP;

/**
 * @brief A survival mini-game where player controls a monster to fight enemies
 */
class Survival : public Scene {
public:
    static Survival* get() {
        static Survival instance;
        return &instance;
    }

    void init() override;
    void update() override;
    void draw() override;
    void end() override;

private:
    Survival();

    // Game states
    enum class GameState {
        PLAYING,
        GAME_OVER
    };

    // Monster structure for survival game
    struct SurvivalMonster {
        float x, y;           // Current position
        float tx, ty;         // Target position
        float vx, vy;         // Velocity for dash
        float speed;          // Normal movement speed
        float dashSpeed;      // Fast movement speed
        float radius;         // Collision radius
        bool isDashing;       // Currently dashing?
        bool isAlive;         // Is monster alive?
        bool isPlayer;        // Is this the player's monster?
        int monsterType;      // Index to monster sprite
        std::string spritePath; // Path to monster sprite
        
        // Dash trail for collision (start and end points)
        float dashStartX, dashStartY;
        float dashEndX, dashEndY;
        bool hasDashTrail;
        
        // AI related
        float shootCooldown;
        float dashCooldown;
        float decisionTimer;
        
        SurvivalMonster() : x(0), y(0), tx(0), ty(0), vx(0), vy(0),
            speed(3.0f), dashSpeed(25.0f), radius(25.0f),
            isDashing(false), isAlive(true), isPlayer(false), monsterType(0),
            dashStartX(0), dashStartY(0), dashEndX(0), dashEndY(0), hasDashTrail(false),
            shootCooldown(0), dashCooldown(0), decisionTimer(0) {}
    };

    // Bullet structure
    struct Bullet {
        float x, y;
        float vx, vy;
        float radius;
        bool isPlayerBullet;
        bool isActive;
        
        Bullet() : x(0), y(0), vx(0), vy(0), radius(5.0f), 
            isPlayerBullet(false), isActive(false) {}
    };

    // Particle structure (decorative dots)
    struct Particle {
        float x, y;
        float vx, vy;
        float radius;
        float friction;
        
        Particle() : x(0), y(0), vx(0), vy(0), radius(3.0f), friction(0.95f) {}
    };

    // Game data
    GameState gameState;
    SurvivalMonster player;
    std::vector<SurvivalMonster> enemies;
    std::vector<Bullet> bullets;
    std::vector<Particle> particles;
    
    // Game stats
    int score;
    int highScore;
    float gameTime;         // Time elapsed in seconds
    float spawnTimer;       // Timer for enemy spawning
    float spawnInterval;    // Time between spawns
    
    // Constants
    static constexpr int FIELD_WIDTH = 1280;
    static constexpr int FIELD_HEIGHT = 720;
    static constexpr int PARTICLE_COUNT = 200;
    static constexpr float BULLET_SPEED = 10.0f;
    
    // Helper functions
    void spawnPlayer();
    void spawnEnemy();
    void spawnBullet(float x, float y, float targetX, float targetY, bool isPlayerBullet);
    void initParticles();
    
    void updatePlayer();
    void updateEnemies();
    void updateBullets();
    void updateParticles();
    void checkCollisions();
    
    void updateEnemyAI(SurvivalMonster& enemy);
    int getAILevel(); // 0 = simple, 1 = medium, 2 = hard
    
    float distance(float x1, float y1, float x2, float y2);
    bool lineCircleCollision(float x1, float y1, float x2, float y2, float cx, float cy, float r);
    
    void drawMonster(const SurvivalMonster& monster);
    void drawBullet(const Bullet& bullet);
    void drawParticle(const Particle& particle);
    void drawUI();
    void drawGameOver();
    
    void resetGame();
    void saveHighScore();
    
    // Button areas for game over screen
    Point playAgainBtn;
    Point menuBtn;
    static constexpr int BUTTON_WIDTH = 200;
    static constexpr int BUTTON_HEIGHT = 50;
};

#endif
