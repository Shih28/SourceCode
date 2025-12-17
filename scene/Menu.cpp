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


void Menu::init(){

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
    auto DC = DataCenter::get_instance();
    
    //background
    auto bg = IC->get("./assets/image/scene/menu.png");
    al_draw_bitmap(bg, 0, 0, 0);

    //facilities
    for(auto &f: pl->getFacilities()){
        f.draw();
    }

    //monsters
    for(auto &m: pl->getMonsters()){
        m.draw();
    }

    //attack, shop, profile with hover effects
    auto atk_pt = Point(102, 620);
    auto shop_pt = Point(1172, 620);
    auto pfp_pt = Point(95, 98);
    
    bool atkHover = atk_pt.overlap(DC->mouse, 90);
    bool pfpHover = pfp_pt.overlap(DC->mouse, 90);
    bool shopHover = DC->mouse.overlap(shop_pt, 90);
    
    auto atk = IC->get(getButtonImage("./assets/image/littleStuff/attack.png", atkHover));
    auto pfp = IC->get(getButtonImage("./assets/image/littleStuff/profile.png", pfpHover));
    auto shop = IC->get(getButtonImage("./assets/image/littleStuff/shop.png", shopHover));
    auto coin = IC->get("./assets/image/littleStuff/coin_bar.png");
    auto berry = IC->get("./assets/image/littleStuff/berry_bar.png");

    al_draw_bitmap(atk, 10, 525, 0);
    al_draw_bitmap(pfp, 5, 5, 0);
    al_draw_bitmap(shop, 1082, 525, 0);
    al_draw_bitmap(coin, 400, 5, 0);
    al_draw_bitmap(berry, 850, 5, 0);

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
