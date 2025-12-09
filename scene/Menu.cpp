#include "Menu.h"
#include "../Utils.h"
#include <vector>
#include "../single_include/nlohmann/json.hpp"
#include "../data/ImageCenter.h"
#include "../facilities/Facility.h"
#include "allegro5/allegro_primitives.h"
#include "../data/DataCenter.h"
#include "../shapes/Circle.h"
#include "../Player.h"


void Menu::init(){

    return;
}

void Menu::update(){
    Player* pl = Player::getPlayer();
    auto DC = DataCenter::get_instance();
    // debug_log("<Menu> updating\n");

    for(auto &f: pl->getFacilities()){
        // debug_log("<Menu> updating f\n");
        f.update();
    }

    auto atk_pt = Point(102, 620);
    auto shop_pt = Point(1172, 620);
    auto pfp_pt = Point(95, 98);
    
    if(atk_pt.overlap(DC->mouse, 90) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        pl->setrequest(Game::STATE::LEVEL);
    }else if(DC->mouse.overlap(shop_pt, 90) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        pl->setrequest(Game::STATE::STORE);
    }else if(pfp_pt.overlap(DC->mouse, 90) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        pl->setrequest(Game::STATE::PROFILE);
    }
}

void Menu::draw(){
    Player* pl = Player::getPlayer();
    auto IC = ImageCenter::get_instance();
    //background
    auto bg = IC->get("./assets/image/scene/menu.png");
    al_draw_bitmap(bg, 0, 0, 0);

    //facilities
    for(auto &f: pl->getFacilities()){
        f.draw();
    }

    //attack, shop, profile
    auto atk = IC->get("./assets/image/littleStuff/attack.png");
    auto pfp = IC->get("./assets/image/littleStuff/profile.png");
    auto shop = IC->get("./assets/image/littleStuff/shop.png");
    auto coin = IC->get("./assets/image/littleStuff/coin_bar.png");
    auto berry = IC->get("./assets/image/littleStuff/berry_bar.png");

    al_draw_bitmap(atk, 10, 525, 0);
    al_draw_bitmap(pfp, 5, 5, 0);
    al_draw_bitmap(shop, 1082, 525, 0);
    al_draw_bitmap(coin, 400, 5, 0);
    al_draw_bitmap(berry, 850, 5, 0);

    // al_draw_circle(102, 620, 90, al_map_rgb(255, 0, 0), 2);
    // al_draw_circle(1172, 620, 90, al_map_rgb(255, 0, 0), 2);
    // al_draw_circle(95, 98, 90, al_map_rgb(255, 0, 0), 2);
    
    
}

void Menu::end(){
    

    return;
}
