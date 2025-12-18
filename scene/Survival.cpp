#include "Survival.h"
#include "../Utils.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../data/FontCenter.h"
#include "../Player.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// Use FontSize namespace for font sizes
using namespace FontSize;

Survival::Survival() : gameState(GameState::PLAYING), score(0), highScore(0),
    gameTime(0), spawnTimer(0), spawnInterval(3.0f),
    playAgainBtn(FIELD_WIDTH / 2 - 110, FIELD_HEIGHT / 2 + 50),
    menuBtn(FIELD_WIDTH / 2 - 110, FIELD_HEIGHT / 2 + 120) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void Survival::init() {
    resetGame();
    highScore = Player::getPlayer()->getGunnerHighScore();
}

void Survival::resetGame() {
    gameState = GameState::PLAYING;
    score = 0;
    gameTime = 0;
    spawnTimer = 0;
    spawnInterval = 3.0f;
    
    enemies.clear();
    bullets.clear();
    
    spawnPlayer();
    initParticles();
    
    // Spawn initial enemies
    for (int i = 0; i < 3; i++) {
        spawnEnemy();
    }
}

void Survival::spawnPlayer() {
    player = SurvivalMonster();
    player.x = FIELD_WIDTH / 2.0f;
    player.y = FIELD_HEIGHT / 2.0f;
    player.tx = player.x;
    player.ty = player.y;
    player.isPlayer = true;
    player.isAlive = true;
    player.speed = 4.0f;
    player.dashSpeed = 30.0f;
    player.radius = 25.0f;
    
    // Get random monster from player's owned monsters
    auto& monsters = Player::getPlayer()->getMonsters();
    if (!monsters.empty()) {
        int idx = std::rand() % monsters.size();
        player.monsterType = idx;
        // Use first monster's type for sprite path
        player.spritePath = "./assets/image/monsters/fire/BadGyaumal/1/default/1.png";
    } else {
        player.spritePath = "./assets/image/monsters/fire/BadGyaumal/1/default/1.png";
    }
}

void Survival::spawnEnemy() {
    SurvivalMonster enemy;
    enemy.isPlayer = false;
    enemy.isAlive = true;
    enemy.speed = 2.0f + (gameTime / 30.0f); // Enemies get faster over time
    enemy.dashSpeed = 20.0f;
    enemy.radius = 25.0f;
    
    // Spawn outside the field
    int side = std::rand() % 4;
    switch (side) {
        case 0: // Top
            enemy.x = static_cast<float>(std::rand() % FIELD_WIDTH);
            enemy.y = -50.0f;
            break;
        case 1: // Bottom
            enemy.x = static_cast<float>(std::rand() % FIELD_WIDTH);
            enemy.y = FIELD_HEIGHT + 50.0f;
            break;
        case 2: // Left
            enemy.x = -50.0f;
            enemy.y = static_cast<float>(std::rand() % FIELD_HEIGHT);
            break;
        case 3: // Right
            enemy.x = FIELD_WIDTH + 50.0f;
            enemy.y = static_cast<float>(std::rand() % FIELD_HEIGHT);
            break;
    }
    
    enemy.tx = player.x;
    enemy.ty = player.y;
    
    // Random monster sprite from available types
    const char* enemySprites[] = {
        "./assets/image/monsters/fire/Freettle/1/default/1.png",
        "./assets/image/monsters/wind/Virelia/1/default/1.png",
        "./assets/image/monsters/fire/BadGyaumal/2/default/1.png"
    };
    enemy.spritePath = enemySprites[std::rand() % 3];
    
    enemy.shootCooldown = 1.0f + static_cast<float>(std::rand() % 20) / 10.0f;
    enemy.dashCooldown = 3.0f + static_cast<float>(std::rand() % 30) / 10.0f;
    enemy.decisionTimer = 0;
    
    enemies.push_back(enemy);
}

void Survival::spawnBullet(float x, float y, float targetX, float targetY, bool isPlayerBullet) {
    Bullet bullet;
    bullet.x = x;
    bullet.y = y;
    bullet.isPlayerBullet = isPlayerBullet;
    bullet.isActive = true;
    bullet.radius = 5.0f;
    
    float dx = targetX - x;
    float dy = targetY - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist > 0) {
        bullet.vx = (dx / dist) * BULLET_SPEED;
        bullet.vy = (dy / dist) * BULLET_SPEED;
    }
    
    bullets.push_back(bullet);
}

void Survival::initParticles() {
    particles.clear();
    particles.resize(PARTICLE_COUNT);
    
    for (auto& p : particles) {
        p.x = static_cast<float>(std::rand() % FIELD_WIDTH);
        p.y = static_cast<float>(std::rand() % FIELD_HEIGHT);
        p.vx = 0;
        p.vy = 0;
        p.radius = 2.0f + static_cast<float>(std::rand() % 3);
    }
}

void Survival::update() {
    DataCenter* DC = DataCenter::get_instance();
    
    if (gameState == GameState::PLAYING) {
        // Update game time (assuming 60 FPS)
        gameTime += 1.0f / 60.0f;
        
        updatePlayer();
        updateEnemies();
        updateBullets();
        updateParticles();
        checkCollisions();
        
        // Spawn timer
        spawnTimer += 1.0f / 60.0f;
        
        // Decrease spawn interval over time (more enemies spawn)
        spawnInterval = std::max(0.5f, 3.0f - gameTime / 20.0f);
        
        if (spawnTimer >= spawnInterval) {
            spawnEnemy();
            spawnTimer = 0;
        }
        
        // Check if player died
        if (!player.isAlive) {
            gameState = GameState::GAME_OVER;
            if (score > highScore) {
                highScore = score;
                saveHighScore();
            }
        }
    } else if (gameState == GameState::GAME_OVER) {
        // Handle game over input
        if (DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
            // Check Play Again button
            if (DC->mouse.x >= playAgainBtn.x && DC->mouse.x <= playAgainBtn.x + BUTTON_WIDTH &&
                DC->mouse.y >= playAgainBtn.y && DC->mouse.y <= playAgainBtn.y + BUTTON_HEIGHT) {
                resetGame();
            }
            // Check Menu button
            else if (DC->mouse.x >= menuBtn.x && DC->mouse.x <= menuBtn.x + BUTTON_WIDTH &&
                     DC->mouse.y >= menuBtn.y && DC->mouse.y <= menuBtn.y + BUTTON_HEIGHT) {
                Player::getPlayer()->setrequest(Game::STATE::MENU);
            }
        }
    }
}

void Survival::updatePlayer() {
    DataCenter* DC = DataCenter::get_instance();
    
    // Handle WASD movement
    bool wasdMoving = false;
    float wasdDx = 0, wasdDy = 0;
    
    if (DC->key_state[ALLEGRO_KEY_W]) { wasdDy -= 1; wasdMoving = true; }
    if (DC->key_state[ALLEGRO_KEY_S]) { wasdDy += 1; wasdMoving = true; }
    if (DC->key_state[ALLEGRO_KEY_A]) { wasdDx -= 1; wasdMoving = true; }
    if (DC->key_state[ALLEGRO_KEY_D]) { wasdDx += 1; wasdMoving = true; }
    
    // Normalize diagonal movement
    if (wasdMoving && wasdDx != 0 && wasdDy != 0) {
        float len = std::sqrt(wasdDx * wasdDx + wasdDy * wasdDy);
        wasdDx /= len;
        wasdDy /= len;
    }
    
    // Handle movement input (left click)
    if (DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
        player.tx = static_cast<float>(DC->mouse.x);
        player.ty = static_cast<float>(DC->mouse.y);
        
        // Check for dash (Ctrl + click)
        if (DC->key_state[ALLEGRO_KEY_LCTRL] || DC->key_state[ALLEGRO_KEY_RCTRL]) {
            player.isDashing = true;
            player.dashStartX = player.x;
            player.dashStartY = player.y;
            player.dashEndX = player.tx;
            player.dashEndY = player.ty;
            player.hasDashTrail = true;
            
            // Calculate dash velocity
            float dx = player.tx - player.x;
            float dy = player.ty - player.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist > 0) {
                player.vx = (dx / dist) * player.dashSpeed;
                player.vy = (dy / dist) * player.dashSpeed;
            }
        }
    }
    
    // Handle shooting (right click)
    if (DC->mouse_state[2] && !DC->prev_mouse_state[2]) {
        spawnBullet(player.x, player.y, 
                   static_cast<float>(DC->mouse.x), 
                   static_cast<float>(DC->mouse.y), true);
    }
    
    // Update position
    if (player.isDashing) {
        player.x += player.vx;
        player.y += player.vy;
        
        // Check if reached target
        float distToTarget = distance(player.x, player.y, player.tx, player.ty);
        if (distToTarget < player.dashSpeed) {
            player.x = player.tx;
            player.y = player.ty;
            player.isDashing = false;
            player.hasDashTrail = false;
            player.vx = 0;
            player.vy = 0;
        }
    } else if (wasdMoving) {
        // WASD movement takes priority over click-to-move
        player.x += wasdDx * player.speed;
        player.y += wasdDy * player.speed;
        // Update target to current position so click-to-move doesn't interfere
        player.tx = player.x;
        player.ty = player.y;
    } else {
        // Normal click-to-move movement
        float dx = player.tx - player.x;
        float dy = player.ty - player.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist > player.speed) {
            player.x += (dx / dist) * player.speed;
            player.y += (dy / dist) * player.speed;
        } else {
            player.x = player.tx;
            player.y = player.ty;
        }
    }
    
    // Clamp to field
    player.x = std::max(player.radius, std::min(static_cast<float>(FIELD_WIDTH) - player.radius, player.x));
    player.y = std::max(player.radius, std::min(static_cast<float>(FIELD_HEIGHT) - player.radius, player.y));
}

void Survival::updateEnemies() {
    for (auto& enemy : enemies) {
        if (!enemy.isAlive) continue;
        
        updateEnemyAI(enemy);
        
        // Update position
        if (enemy.isDashing) {
            enemy.x += enemy.vx;
            enemy.y += enemy.vy;
            
            float distToTarget = distance(enemy.x, enemy.y, enemy.tx, enemy.ty);
            if (distToTarget < enemy.dashSpeed) {
                enemy.x = enemy.tx;
                enemy.y = enemy.ty;
                enemy.isDashing = false;
                enemy.hasDashTrail = false;
                enemy.vx = 0;
                enemy.vy = 0;
            }
        } else {
            float dx = enemy.tx - enemy.x;
            float dy = enemy.ty - enemy.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist > enemy.speed) {
                enemy.x += (dx / dist) * enemy.speed;
                enemy.y += (dy / dist) * enemy.speed;
            }
        }
    }
    
    // Remove dead enemies
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const SurvivalMonster& e) { return !e.isAlive; }),
        enemies.end());
}

void Survival::updateEnemyAI(SurvivalMonster& enemy) {
    int aiLevel = getAILevel();
    enemy.decisionTimer += 1.0f / 60.0f;
    enemy.shootCooldown -= 1.0f / 60.0f;
    enemy.dashCooldown -= 1.0f / 60.0f;
    
    switch (aiLevel) {
        case 0: // Simple - just move toward player and shoot occasionally
            if (enemy.decisionTimer > 0.5f) {
                enemy.tx = player.x;
                enemy.ty = player.y;
                enemy.decisionTimer = 0;
            }
            if (enemy.shootCooldown <= 0 && distance(enemy.x, enemy.y, player.x, player.y) < 400) {
                spawnBullet(enemy.x, enemy.y, player.x, player.y, false);
                enemy.shootCooldown = 2.0f + static_cast<float>(std::rand() % 20) / 10.0f;
            }
            break;
            
        case 1: // Medium - dodge bullets, shoot more often
            if (enemy.decisionTimer > 0.3f) {
                enemy.tx = player.x;
                enemy.ty = player.y;
                
                // Try to dodge nearby bullets
                for (const auto& bullet : bullets) {
                    if (bullet.isPlayerBullet && bullet.isActive) {
                        float bulletDist = distance(bullet.x, bullet.y, enemy.x, enemy.y);
                        if (bulletDist < 100) {
                            // Move perpendicular to bullet direction
                            enemy.tx = enemy.x - bullet.vy * 3;
                            enemy.ty = enemy.y + bullet.vx * 3;
                            break;
                        }
                    }
                }
                enemy.decisionTimer = 0;
            }
            if (enemy.shootCooldown <= 0 && distance(enemy.x, enemy.y, player.x, player.y) < 500) {
                spawnBullet(enemy.x, enemy.y, player.x, player.y, false);
                enemy.shootCooldown = 1.0f + static_cast<float>(std::rand() % 10) / 10.0f;
            }
            break;
            
        case 2: // Hard - predict player, dash, coordinate
            if (enemy.decisionTimer > 0.2f) {
                // Predict player position
                float predictX = player.x + (player.tx - player.x) * 0.5f;
                float predictY = player.y + (player.ty - player.y) * 0.5f;
                enemy.tx = predictX;
                enemy.ty = predictY;
                
                // Dodge bullets
                for (const auto& bullet : bullets) {
                    if (bullet.isPlayerBullet && bullet.isActive) {
                        float bulletDist = distance(bullet.x, bullet.y, enemy.x, enemy.y);
                        if (bulletDist < 150) {
                            enemy.tx = enemy.x - bullet.vy * 5;
                            enemy.ty = enemy.y + bullet.vx * 5;
                            break;
                        }
                    }
                }
                
                // Consider dashing
                if (enemy.dashCooldown <= 0 && distance(enemy.x, enemy.y, player.x, player.y) < 300) {
                    enemy.isDashing = true;
                    enemy.dashStartX = enemy.x;
                    enemy.dashStartY = enemy.y;
                    enemy.dashEndX = player.x;
                    enemy.dashEndY = player.y;
                    enemy.hasDashTrail = true;
                    
                    float dx = player.x - enemy.x;
                    float dy = player.y - enemy.y;
                    float dist = std::sqrt(dx * dx + dy * dy);
                    if (dist > 0) {
                        enemy.vx = (dx / dist) * enemy.dashSpeed;
                        enemy.vy = (dy / dist) * enemy.dashSpeed;
                    }
                    enemy.dashCooldown = 5.0f;
                }
                
                enemy.decisionTimer = 0;
            }
            if (enemy.shootCooldown <= 0) {
                // Lead the shot
                float dist = distance(enemy.x, enemy.y, player.x, player.y);
                float leadTime = dist / BULLET_SPEED;
                float leadX = player.x + (player.tx - player.x) * leadTime * 0.1f;
                float leadY = player.y + (player.ty - player.y) * leadTime * 0.1f;
                spawnBullet(enemy.x, enemy.y, leadX, leadY, false);
                enemy.shootCooldown = 0.5f + static_cast<float>(std::rand() % 5) / 10.0f;
            }
            break;
    }
}

int Survival::getAILevel() {
    if (gameTime < 10.0f) return 0;      // Simple
    else if (gameTime < 20.0f) return 1; // Medium
    else return 2;                        // Hard
}

void Survival::updateBullets() {
    for (auto& bullet : bullets) {
        if (!bullet.isActive) continue;
        
        bullet.x += bullet.vx;
        bullet.y += bullet.vy;
        
        // Deactivate if out of bounds
        if (bullet.x < -50 || bullet.x > FIELD_WIDTH + 50 ||
            bullet.y < -50 || bullet.y > FIELD_HEIGHT + 50) {
            bullet.isActive = false;
        }
    }
    
    // Remove inactive bullets
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.isActive; }),
        bullets.end());
}

void Survival::updateParticles() {
    // Push particles away from monsters
    auto pushParticles = [this](float mx, float my, float radius) {
        for (auto& p : particles) {
            float dist = distance(p.x, p.y, mx, my);
            if (dist < radius + 20 && dist > 0) {
                float pushForce = (radius + 20 - dist) / 5.0f;
                float dx = p.x - mx;
                float dy = p.y - my;
                p.vx += (dx / dist) * pushForce;
                p.vy += (dy / dist) * pushForce;
            }
        }
    };
    
    // Push from player
    pushParticles(player.x, player.y, player.radius);
    
    // Push from enemies
    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            pushParticles(enemy.x, enemy.y, enemy.radius);
        }
    }
    
    // Update particle positions
    for (auto& p : particles) {
        p.x += p.vx;
        p.y += p.vy;
        p.vx *= p.friction;
        p.vy *= p.friction;
        
        // Keep particles in bounds
        if (p.x < 0) { p.x = 0; p.vx = -p.vx * 0.5f; }
        if (p.x > FIELD_WIDTH) { p.x = FIELD_WIDTH; p.vx = -p.vx * 0.5f; }
        if (p.y < 0) { p.y = 0; p.vy = -p.vy * 0.5f; }
        if (p.y > FIELD_HEIGHT) { p.y = FIELD_HEIGHT; p.vy = -p.vy * 0.5f; }
    }
}

void Survival::checkCollisions() {
    // Check player bullets hitting enemies
    for (auto& bullet : bullets) {
        if (!bullet.isActive || !bullet.isPlayerBullet) continue;
        
        for (auto& enemy : enemies) {
            if (!enemy.isAlive) continue;
            
            if (distance(bullet.x, bullet.y, enemy.x, enemy.y) < enemy.radius + bullet.radius) {
                enemy.isAlive = false;
                bullet.isActive = false;
                score++;
                break;
            }
        }
    }
    
    // Check enemy bullets hitting player (player is invincible during dash)
    for (auto& bullet : bullets) {
        if (!bullet.isActive || bullet.isPlayerBullet) continue;
        
        if (!player.isDashing && distance(bullet.x, bullet.y, player.x, player.y) < player.radius + bullet.radius) {
            player.isAlive = false;
            bullet.isActive = false;
            break;
        }
    }
    
    // Check dash trails
    // Player dash trail hitting enemies
    if (player.hasDashTrail) {
        for (auto& enemy : enemies) {
            if (!enemy.isAlive) continue;
            
            if (lineCircleCollision(player.dashStartX, player.dashStartY,
                                   player.x, player.y,
                                   enemy.x, enemy.y, enemy.radius)) {
                enemy.isAlive = false;
                score++;
            }
        }
    }
    
    // Enemy dash trails hitting player (player is invincible during dash)
    for (auto& enemy : enemies) {
        if (!enemy.isAlive || !enemy.hasDashTrail) continue;
        
        if (!player.isDashing && lineCircleCollision(enemy.dashStartX, enemy.dashStartY,
                               enemy.x, enemy.y,
                               player.x, player.y, player.radius)) {
            player.isAlive = false;
        }
    }
    
    // Enemy dash trails hitting other enemies (friendly fire for dash)
    for (size_t i = 0; i < enemies.size(); i++) {
        if (!enemies[i].isAlive || !enemies[i].hasDashTrail) continue;
        
        for (size_t j = 0; j < enemies.size(); j++) {
            if (i == j || !enemies[j].isAlive) continue;
            
            if (lineCircleCollision(enemies[i].dashStartX, enemies[i].dashStartY,
                                   enemies[i].x, enemies[i].y,
                                   enemies[j].x, enemies[j].y, enemies[j].radius)) {
                enemies[j].isAlive = false;
                score++;
            }
        }
    }
}

float Survival::distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

bool Survival::lineCircleCollision(float x1, float y1, float x2, float y2, 
                                    float cx, float cy, float r) {
    // Vector from line start to circle center
    float dx = x2 - x1;
    float dy = y2 - y1;
    float fx = x1 - cx;
    float fy = y1 - cy;
    
    float a = dx * dx + dy * dy;
    float b = 2 * (fx * dx + fy * dy);
    float c = fx * fx + fy * fy - r * r;
    
    float discriminant = b * b - 4 * a * c;
    
    if (discriminant < 0) return false;
    
    discriminant = std::sqrt(discriminant);
    float t1 = (-b - discriminant) / (2 * a);
    float t2 = (-b + discriminant) / (2 * a);
    
    // Check if intersection is within line segment
    return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
}

void Survival::draw() {
    // Draw white background
    al_clear_to_color(al_map_rgb(255, 255, 255));
    
    // Draw particles first (background)
    for (const auto& p : particles) {
        drawParticle(p);
    }
    
    // Draw dash trails
    if (player.hasDashTrail && player.isAlive) {
        al_draw_line(player.dashStartX, player.dashStartY, player.x, player.y,
                    al_map_rgba(0, 100, 255, 150), 5.0f);
    }
    for (const auto& enemy : enemies) {
        if (enemy.hasDashTrail && enemy.isAlive) {
            al_draw_line(enemy.dashStartX, enemy.dashStartY, enemy.x, enemy.y,
                        al_map_rgba(255, 100, 0, 150), 5.0f);
        }
    }
    
    // Draw bullets
    for (const auto& bullet : bullets) {
        if (bullet.isActive) {
            drawBullet(bullet);
        }
    }
    
    // Draw enemies
    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            drawMonster(enemy);
        }
    }
    
    // Draw player
    if (player.isAlive) {
        drawMonster(player);
    }
    
    // Draw UI
    drawUI();
    
    // Draw game over screen if needed
    if (gameState == GameState::GAME_OVER) {
        drawGameOver();
    }
}

void Survival::drawMonster(const SurvivalMonster& monster) {
    ImageCenter* IC = ImageCenter::get_instance();
    
    ALLEGRO_BITMAP* sprite = IC->get(monster.spritePath);
    if (sprite) {
        int w = al_get_bitmap_width(sprite);
        int h = al_get_bitmap_height(sprite);
        float scale = (monster.radius * 2) / static_cast<float>(std::max(w, h));
        
        al_draw_scaled_bitmap(sprite, 
            0, 0, w, h,
            monster.x - monster.radius, monster.y - monster.radius,
            monster.radius * 2, monster.radius * 2, 0);
    } else {
        // Fallback: draw colored circle
        ALLEGRO_COLOR color = monster.isPlayer ? 
            al_map_rgb(0, 150, 255) : al_map_rgb(255, 80, 80);
        al_draw_filled_circle(monster.x, monster.y, monster.radius, color);
        al_draw_circle(monster.x, monster.y, monster.radius, al_map_rgb(0, 0, 0), 2.0f);
    }
}

void Survival::drawBullet(const Bullet& bullet) {
    ALLEGRO_COLOR color = bullet.isPlayerBullet ? 
        al_map_rgb(0, 100, 255) : al_map_rgb(255, 50, 50);
    al_draw_filled_circle(bullet.x, bullet.y, bullet.radius, color);
    al_draw_circle(bullet.x, bullet.y, bullet.radius, al_map_rgb(0, 0, 0), 1.0f);
}

void Survival::drawParticle(const Particle& particle) {
    al_draw_filled_circle(particle.x, particle.y, particle.radius, 
                         al_map_rgba(200, 200, 200, 200));
}

void Survival::drawUI() {
    FontCenter* FC = FontCenter::get_instance();
    
    // Draw score
    char scoreText[64];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    al_draw_text(FC->caviar_dreams[MEDIUM], al_map_rgb(0, 0, 0),
                10, 10, ALLEGRO_ALIGN_LEFT, scoreText);
    
    // Draw high score
    char highScoreText[64];
    snprintf(highScoreText, sizeof(highScoreText), "High Score: %d", highScore);
    al_draw_text(FC->caviar_dreams[MEDIUM], al_map_rgb(100, 100, 100),
                10, 40, ALLEGRO_ALIGN_LEFT, highScoreText);
    
    // Draw timer
    char timeText[64];
    snprintf(timeText, sizeof(timeText), "Time: %.1fs", gameTime);
    al_draw_text(FC->caviar_dreams[MEDIUM], al_map_rgb(0, 0, 0),
                FIELD_WIDTH - 10, 10, ALLEGRO_ALIGN_RIGHT, timeText);
    
    // Draw AI level indicator
    const char* aiLevelText[] = {"AI: Easy", "AI: Medium", "AI: Hard"};
    int aiLevel = getAILevel();
    ALLEGRO_COLOR aiColor = aiLevel == 0 ? al_map_rgb(0, 150, 0) :
                            aiLevel == 1 ? al_map_rgb(200, 150, 0) :
                                          al_map_rgb(200, 0, 0);
    al_draw_text(FC->caviar_dreams[SMALL], aiColor,
                FIELD_WIDTH - 10, 40, ALLEGRO_ALIGN_RIGHT, aiLevelText[aiLevel]);
    
    // Draw controls hint
    al_draw_text(FC->caviar_dreams[SMALL], al_map_rgb(100, 100, 100),
                FIELD_WIDTH / 2, FIELD_HEIGHT - 25, ALLEGRO_ALIGN_CENTER,
                "WASD/Click: Move | Ctrl+Click: Dash | Right Click: Shoot");
}

void Survival::drawGameOver() {
    DataCenter* DC = DataCenter::get_instance();
    FontCenter* FC = FontCenter::get_instance();
    
    // Semi-transparent overlay
    al_draw_filled_rectangle(0, 0, FIELD_WIDTH, FIELD_HEIGHT, al_map_rgba(0, 0, 0, 150));
    
    // Game Over text
    al_draw_text(FC->caviar_dreams[LARGE], al_map_rgb(255, 255, 255),
                FIELD_WIDTH / 2, FIELD_HEIGHT / 2 - 100, ALLEGRO_ALIGN_CENTER, "GAME OVER");
    
    // Final score
    char scoreText[64];
    snprintf(scoreText, sizeof(scoreText), "Final Score: %d", score);
    al_draw_text(FC->caviar_dreams[MEDIUM], al_map_rgb(255, 255, 255),
                FIELD_WIDTH / 2, FIELD_HEIGHT / 2 - 40, ALLEGRO_ALIGN_CENTER, scoreText);
    
    // High score
    if (score >= highScore) {
        al_draw_text(FC->caviar_dreams[MEDIUM], al_map_rgb(255, 215, 0),
                    FIELD_WIDTH / 2, FIELD_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "NEW HIGH SCORE!");
    }
    
    // Buttons
    bool playAgainHover = DC->mouse.x >= playAgainBtn.x && DC->mouse.x <= playAgainBtn.x + BUTTON_WIDTH &&
                          DC->mouse.y >= playAgainBtn.y && DC->mouse.y <= playAgainBtn.y + BUTTON_HEIGHT;
    bool menuHover = DC->mouse.x >= menuBtn.x && DC->mouse.x <= menuBtn.x + BUTTON_WIDTH &&
                     DC->mouse.y >= menuBtn.y && DC->mouse.y <= menuBtn.y + BUTTON_HEIGHT;
    
    // Play Again button
    ALLEGRO_COLOR playAgainColor = playAgainHover ? al_map_rgb(100, 200, 100) : al_map_rgb(80, 160, 80);
    al_draw_filled_rectangle(playAgainBtn.x, playAgainBtn.y, 
                            playAgainBtn.x + BUTTON_WIDTH, playAgainBtn.y + BUTTON_HEIGHT,
                            playAgainColor);
    al_draw_rectangle(playAgainBtn.x, playAgainBtn.y,
                     playAgainBtn.x + BUTTON_WIDTH, playAgainBtn.y + BUTTON_HEIGHT,
                     al_map_rgb(255, 255, 255), 2.0f);
    al_draw_text(FC->caviar_dreams[MEDIUM], al_map_rgb(255, 255, 255),
                playAgainBtn.x + BUTTON_WIDTH / 2, playAgainBtn.y + 10,
                ALLEGRO_ALIGN_CENTER, "Play Again");
    
    // Menu button
    ALLEGRO_COLOR menuColor = menuHover ? al_map_rgb(100, 100, 200) : al_map_rgb(80, 80, 160);
    al_draw_filled_rectangle(menuBtn.x, menuBtn.y,
                            menuBtn.x + BUTTON_WIDTH, menuBtn.y + BUTTON_HEIGHT,
                            menuColor);
    al_draw_rectangle(menuBtn.x, menuBtn.y,
                     menuBtn.x + BUTTON_WIDTH, menuBtn.y + BUTTON_HEIGHT,
                     al_map_rgb(255, 255, 255), 2.0f);
    al_draw_text(FC->caviar_dreams[MEDIUM], al_map_rgb(255, 255, 255),
                menuBtn.x + BUTTON_WIDTH / 2, menuBtn.y + 10,
                ALLEGRO_ALIGN_CENTER, "Menu");
}

void Survival::saveHighScore() {
    Player::getPlayer()->setGunnerHighScore(highScore);
    Player::getPlayer()->savePlayerData();
}

void Survival::end() {
    // Cleanup if needed
}
