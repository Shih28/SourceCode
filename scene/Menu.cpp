#include "Menu.h"
#include "../Utils.h"
#include <vector>
#include "../single_include/nlohmann/json.hpp"
#include "../facilities/Facility.h"
#include "../Player.h"


void Menu::init(){

    return;
}

void Menu::update(){
    Player* pl = Player::getPlayer();
    // debug_log("<Menu> updating\n");

    for(auto &f: pl->getFacilities()){
        // debug_log("<Menu> updating f\n");
        f.update();
    }
}

void Menu::draw(){
    Player* pl = Player::getPlayer();
    // debug_log("<Menu> start drawing\n");
    for(auto &f: pl->getFacilities()){
        f.draw();
    }
}

void Menu::end(){
    

    return;
}
