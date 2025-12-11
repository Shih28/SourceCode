#include "Store.h"
#include "../Player.h"
#include "../Monster.h"
#include "../data/ImageCenter.h"
#include <utility>
#include "allegro5/allegro_primitives.h"
#include "../data/DataCenter.h"
#include "../shapes/Point.h"
#include "../Utils.h"
#include <vector>

int Store::page = 0;
const int MAX_NUM = 4;
const std::pair<int, int> MONS_POS[4] ={
    {100, 300},
    {400, 300},
    {700, 300},
    {1100, 300}
};

const int WIDTH = 80;
const int HEIGHT = 150;

void Store::init(){
    state = ALL;
    page=0;
}

void Store::update(){
    auto pl = Player::getPlayer();
    
    auto DC = DataCenter::get_instance();
    //exit
    if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        pl->setrequest(Game::STATE::MENU);

        //owned monsters update
        for(auto &m: pl->getMonsters()){
            if(m.getPlacing()==Monster::PLACE_M::FEEDING){
                m.setPlacing(Monster::PLACE_M::HABITAT);
            }
        }
    }

    //monsters
    auto &lib = pl->getAllMonsters();  
    for(int i=0; i<MAX_NUM; i++){
        if(Rectangle(MONS_POS[i].first, MONS_POS[i].second,
            MONS_POS[i].first+WIDTH, MONS_POS[i].second+HEIGHT).overlap(DC->mouse)
            && DC->mouse_state[1] && !DC->prev_mouse_state[1]){

                int libIdx = monsters_in_display_idx[i];
                // validate index
                if(libIdx < 0 || libIdx >= (int)lib.size()){
                    debug_log("WARN: purchase clicked but no monster mapped for slot %d (idx=%d)\n", i, libIdx);
                    continue;
                }
                // copy the template monster from all_monsters into the player's owned list.
                // images are shared via Monster::registerTypeImages() and maps, so no per-instance image copies needed.
                Monster *m = new Monster();
                auto &mask = lib[libIdx];

                m->setType(mask.getType());
                m->setSpecies(mask.getSpecies());
                pl->getMonsters().push_back(*m);
                 debug_log("idx: %d\n", pl->getMonsters()[0].getPlacing());
                 debug_log("PURCHESS!\n");
                 state = SUCESS_PUR;
            }
    }

}

void Store::draw(){
    Player *pl = Player::getPlayer();
    auto &lib = pl->getAllMonsters();                  
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
        debug_log("lib size: %d\n", lib.size());
        // draw up to MAX_NUM items on the current page and populate display index mapping
        int start = page * MAX_NUM;
        for(int slot = 0; slot < MAX_NUM; ++slot){
            int libIdx = start + slot;
            if(libIdx >= (int)lib.size()){
                monsters_in_display_idx[slot] = -1;
                continue;
            }
            monsters_in_display_idx[slot] = libIdx;
            al_draw_bitmap(lib[libIdx].getImgInStore(), MONS_POS[slot].first, MONS_POS[slot].second, 0);
            al_draw_rectangle(MONS_POS[slot].first, MONS_POS[slot].second,
                MONS_POS[slot].first+WIDTH, MONS_POS[slot].second+HEIGHT, al_map_rgb(255,0,0), 2);
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

void Store::updateMonstersInDisplay(){
    auto *pl = Player::getPlayer();
    auto &lib = pl->getAllMonsters(); // reference to real container

    // reset indices
    for(int k=0; k<MAX_NUM; ++k) monsters_in_display_idx[k] = -1;

    int start = page * MAX_NUM;
    int j = 0;
    for(int i = start; i < lib.size() && j < MAX_NUM; ++i, ++j){
        monsters_in_display_idx[j] = i;
    }
}
