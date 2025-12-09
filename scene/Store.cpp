#include "Store.h"
#include "../Player.h"
#include "../Monster.h"
#include "../data/ImageCenter.h"
#include <utility>
#include "allegro5/allegro_primitives.h"
#include "../data/DataCenter.h"
#include "../shapes/Point.h"
#include <vector>

int Store::page = 0;
//TODO
std::pair<int, int> MONS_POS[8];

void Store::init(){
    page=0;
}

void Store::update(){
    auto pl = Player::getPlayer();
    auto DC = DataCenter::get_instance();
    //exit  
    if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        pl->setrequest(Game::STATE::MENU);
    }

}

void Store::draw(){
    Player *pl = Player::getPlayer();
    std::vector<Monster> lib = pl->getAllMonsters();
    auto IC = ImageCenter::get_instance();
    auto bg = IC->get("./assets/image/scene/shop.png");

    //background
    al_draw_bitmap(bg, 0, 0, 0);

    //exit
    auto exit = IC->get("./assets/image/littleStuff/exit.png");
    al_draw_bitmap(exit, 1100, 10, 0);

    //monsters
    switch (state){
    case ALL:{
        for(int i=page*8; i<lib.size(); i++){
            al_draw_bitmap(lib[i].getImgInStore(), MONS_POS[i].first, MONS_POS[i].second, 0);
        }
        break;
    }
    case FIRE:{
        page=0;
        int i=0;
        for(auto &m: lib){
            if(m.getSpecies()==Monster::SPECIES_M::FIRE){
                al_draw_bitmap(m.getImgInStore(), MONS_POS[i].first, MONS_POS[i].second, 0);
                i++;
            }
        }
        break;
    }
    case WATER:{
        page=0;
        int i=0;
        for(auto &m: lib){
            if(m.getSpecies()==Monster::SPECIES_M::WATER){
                al_draw_bitmap(m.getImgInStore(), MONS_POS[i].first, MONS_POS[i].second, 0);
                i++;
            }
        }
        break;
    }
    case WIND:{
        page=0;
        int i=0;
        for(auto &m: lib){
            if(m.getSpecies()==Monster::SPECIES_M::WIND){
                al_draw_bitmap(m.getImgInStore(), MONS_POS[i].first, MONS_POS[i].second, 0);
                i++;
            }
        }
        break;
    }
    case LIGHTNING:{
        page=0;
        int i=0;
        for(auto &m: lib){
            if(m.getSpecies()==Monster::SPECIES_M::LIGHTNING){
                al_draw_bitmap(m.getImgInStore(), MONS_POS[i].first, MONS_POS[i].second, 0);
                i++;
            }
        }
        break;
    }
    case BUY_ATTEMPT:{
        page=0;
        
        break;
    }
    case SUCESS_PUR:{
        page=0;
        
        break;
    }
    case FAIL_PUR:{
        page=0;
        
        break;
    }
    
    }

    //hitboxes
    al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);

}

void Store::end(){

}