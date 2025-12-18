#include "Menu.h"
#include "../Utils.h"
#include <vector>
#include "../single_include/nlohmann/json.hpp"
#include "../data/ImageCenter.h"
#include "../facilities/Facility.h"
#include "../data/FontCenter.h"
#include "allegro5/allegro_primitives.h"
#include "../data/DataCenter.h"
#include "../shapes/Circle.h"
#include "../Player.h"
#include <string>
#include "allegro5/allegro_font.h"

// Helper function to get button image with hover effect
static std::string getButtonImage(const std::string& basePath, bool hovering) {
    if (!hovering) return basePath;
    size_t dotPos = basePath.rfind(".png");
    if (dotPos != std::string::npos) {
        return basePath.substr(0, dotPos) + "2.png";
    }
    return basePath;
}

const int MOVEMENT_SPEED = 4;
const int START_X_POS_FOR_MENU = 800;
const int START_Y_POS_FOR_MENU = 330;
const int BG_RIGHT_END = 1800;
const int BG_DOWN_END = 1100;

void Menu::init(){
    return;
}

void Menu::veryInit(){
    x = START_X_POS_FOR_MENU;
    y = START_Y_POS_FOR_MENU;
    return;
}

void Menu::update(){
    Player* pl = Player::getPlayer();
    auto DC = DataCenter::get_instance();

    for(auto &m: pl->getMonsters()){
        m.update();
    }

    for(auto &f: pl->getFacilities()){
        f.update();
    }

    auto atk_pt = Point(102, 620);
    auto shop_pt = Point(1172, 620);
    auto pfp_pt = Point(95, 98);
    auto survival_pt = Point(250, 620);  // Left bottom, next to attack button
    
    if(atk_pt.overlap(DC->mouse, 90) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        pl->setrequest(Game::STATE::LEVEL);
    }else if(DC->mouse.overlap(shop_pt, 90) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        pl->setrequest(Game::STATE::STORE);
    }else if(pfp_pt.overlap(DC->mouse, 90) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        pl->setrequest(Game::STATE::PROFILE);
    }else if(survival_pt.overlap(DC->mouse, 60) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        pl->setrequest(Game::STATE::SURVIVAL);
    }

    auto &fec = pl->getFacilities();
    auto &mons = pl->getMonsters();

    //menu bg manipulation 
    if(DC->key_state[ALLEGRO_KEY_S]){
        y += MOVEMENT_SPEED;
        if(y>BG_DOWN_END){
            y = BG_DOWN_END;
        }else{
            for(auto &f: fec){
                f.getY() -= MOVEMENT_SPEED;
                for(int i=0; i<2; i++){
                    if(f.getHaveMonsters(i)){
                        auto &m = mons[f.getMonsterIndex(i)];
                        m.setFacilityRec(Rectangle(f.getX(), f.getY(), 
                        f.getX()+f.width, f.getY()+f.length));
                        m.getY() -= MOVEMENT_SPEED;
                    }
                }
            }

        }
    }else if(DC->key_state[ALLEGRO_KEY_W]){
        y -= MOVEMENT_SPEED;
        if(y<0){
            y=0;
        }else{
            for(auto &f: fec){
                f.getY() += MOVEMENT_SPEED;
                for(int i=0; i<2; i++){
                    if(f.getHaveMonsters(i)){
                        auto &m = mons[f.getMonsterIndex(i)];
                        m.setFacilityRec(Rectangle(f.getX(), f.getY(), 
                        f.getX()+f.width, f.getY()+f.length));
                        m.getY() += MOVEMENT_SPEED;
                    }
                }
            }
        }
    }else if(DC->key_state[ALLEGRO_KEY_D]){
        x += MOVEMENT_SPEED;
        if(x<0){
            x=0;
        }else{
            for(auto &f: fec){
                f.getX() -= MOVEMENT_SPEED;
                for(int i=0; i<2; i++){
                    if(f.getHaveMonsters(i)){
                        auto &m = mons[f.getMonsterIndex(i)];
                        m.setFacilityRec(Rectangle(f.getX(), f.getY(), 
                        f.getX()+f.width, f.getY()+f.length));
                        m.getX() -= MOVEMENT_SPEED;
                    }
                }
            }
        }
    }else if(DC->key_state[ALLEGRO_KEY_A]){
        x -= MOVEMENT_SPEED;
        if(x>BG_RIGHT_END){
            x=BG_RIGHT_END;
        }else{
            for(auto &f: fec){
                f.getX() += MOVEMENT_SPEED;
                for(int i=0; i<2; i++){
                    if(f.getHaveMonsters(i)){
                        auto &m = mons[f.getMonsterIndex(i)];
                        m.setFacilityRec(Rectangle(f.getX(), f.getY(), 
                        f.getX()+f.width, f.getY()+f.length));
                        m.getX() += MOVEMENT_SPEED;
                    }
                }
            }
        }
    }
}

void Menu::draw(){
    Player* pl = Player::getPlayer();
    auto IC = ImageCenter::get_instance();
    auto DC = DataCenter::get_instance();
    
    //background
    auto bg = IC->get("./assets/image/scene/menu.png");
    al_draw_bitmap_region(bg, x, y, 1280, 720, 0, 0, 0);
    // al_draw_bitmap(bg, 0, 0, 0);

    //facilities
    for(auto &f: pl->getFacilities()){
        f.draw();
    }

    //monsters
    for(auto &m: pl->getMonsters()){
        m.draw();
    }

    for(auto &f: pl->getFacilities()){
        if(f.getInVal()){
            char str[] = "The farm is occupied!";
            al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(255,255,255), 640, 360, ALLEGRO_ALIGN_CENTRE, str);
        }
    }

    //attack, shop, profile with hover effects
    auto atk_pt = Point(102, 620);
    auto shop_pt = Point(1172, 620);
    auto pfp_pt = Point(95, 98);
    auto survival_pt = Point(250, 620);
    
    bool atkHover = atk_pt.overlap(DC->mouse, 90);
    bool pfpHover = pfp_pt.overlap(DC->mouse, 90);
    bool shopHover = DC->mouse.overlap(shop_pt, 90);
    bool survivalHover = survival_pt.overlap(DC->mouse, 60);
    
    auto atk = IC->get(getButtonImage("./assets/image/littleStuff/attack.png", atkHover));
    auto pfp = IC->get(getButtonImage("./assets/image/littleStuff/profile.png", pfpHover));
    auto shop = IC->get(getButtonImage("./assets/image/littleStuff/shop.png", shopHover));
    auto coin = IC->get("./assets/image/littleStuff/coin_bar.png");
    auto berry = IC->get("./assets/image/littleStuff/berry_bar.png");
    auto survivalBtn = IC->get("./assets/image/levelmenu/button.png");

    al_draw_bitmap(atk, 10, 525, 0);
    al_draw_bitmap(pfp, 5, 5, 0);
    al_draw_bitmap(shop, 1082, 525, 0);
    al_draw_bitmap(coin, 400, 5, 0);
    al_draw_bitmap(berry, 850, 5, 0);
    
    // Draw survival button at left bottom (scaled and positioned)
    if(survivalBtn) {
        int btnW = al_get_bitmap_width(survivalBtn);
        int btnH = al_get_bitmap_height(survivalBtn);
        float scale = 80.0f / btnH;  // Scale to 80px height
        float drawX = 210;
        float drawY = 580;
        al_draw_scaled_bitmap(survivalBtn, 0, 0, btnW, btnH, 
                              drawX, drawY, btnW * scale, btnH * scale, 0);
        
        // Draw "Survival" text on button
        auto FC = FontCenter::get_instance();
        ALLEGRO_COLOR textColor = survivalHover ? al_map_rgb(255, 255, 100) : al_map_rgb(255, 255, 255);
        al_draw_text(FC->caviar_dreams[FontSize::SMALL], textColor, 
                    drawX + (btnW * scale) / 2, drawY + (btnH * scale) / 2 - 6,
                    ALLEGRO_ALIGN_CENTER, "Survival");
    }

    //draw number of berries and coins
    std::string c, b;
    auto FC = FontCenter::get_instance();
    b = std::to_string(pl->getBer());
    c = std::to_string(pl->getCoin());
    
    al_draw_text(FC->caviar_dreams[36], al_map_rgb(0,0,0), 500, 15, 0, c.c_str());
    al_draw_text(FC->caviar_dreams[36], al_map_rgb(0,0,0), 960, 15, 0, b.c_str());
    
    
}

void Menu::end(){
    

    return;
}
