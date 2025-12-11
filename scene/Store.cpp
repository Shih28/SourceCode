#include "Store.h"
#include "../Player.h"
#include "../Monster.h"
#include "../data/ImageCenter.h"
#include <utility>
#include "allegro5/allegro_primitives.h"
#include "../data/DataCenter.h"
#include "../shapes/Point.h"
#include "../data/FontCenter.h"
#include "allegro5/allegro_font.h"
#include "../Utils.h"
#include <vector>

int Store::page = 0;
const int MAX_NUM = 4;

// Helper function to get button image with hover effect
static std::string getButtonImage(const std::string& basePath, bool hovering) {
    if (!hovering) return basePath;
    size_t dotPos = basePath.rfind(".png");
    if (dotPos != std::string::npos) {
        return basePath.substr(0, dotPos) + "2.png";
    }
    return basePath;
}

// Screen and monster dimensions
const int SCREEN_WIDTH = 1280;
const int WIDTH = 80;
const int HEIGHT = 150;

// Calculate evenly spaced positions for 4 monsters in a single row
const int HORIZONTAL_SPACING = 260;  // Space between each monster
const int GRID_WIDTH = (MAX_NUM - 1) * HORIZONTAL_SPACING + WIDTH;
const int START_X = (SCREEN_WIDTH - GRID_WIDTH) / 2;
const int Y_POSITION = 300;

// Generate positions using math instead of hardcoding
const std::pair<int, int> MONS_POS[4] = {
    {START_X + 0 * HORIZONTAL_SPACING, Y_POSITION},
    {START_X + 1 * HORIZONTAL_SPACING, Y_POSITION},
    {START_X + 2 * HORIZONTAL_SPACING, Y_POSITION},
    {START_X + 3 * HORIZONTAL_SPACING, Y_POSITION}
};

void Store::init(){
    state = ALL;
    noti_cnt=0;
    page=0;
}

void Store::update(){
    auto pl = Player::getPlayer();
    auto DC = DataCenter::get_instance();
    updateMonstersInDisplay();
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
                if(libIdx < 0 || libIdx >= (int)lib.size()){
                    continue;
                }
                auto &mask = lib[libIdx];
                int &coin = pl->getCoin();

                if(coin >= mask.getPrice()){
                    coin -= mask.getPrice();
                    Monster *m = new Monster();
                
                    m->setType(mask.getType());
                    m->setSpecies(mask.getSpecies());
                    pl->getMonsters().push_back(*m);
                    noti = SUCCESS;
                }else{
                    noti = FAIL;
                }
                pre_state = state;      
                state = PUR_NOTI;
            }
    }

    if(state==PUR_NOTI){
        if(noti_cnt<=60){
            noti_cnt++;
        }else{
            state = pre_state;
            noti_cnt=0;
        }
    }

}

void Store::draw(){
    Player *pl = Player::getPlayer();
    auto &lib = pl->getAllMonsters();                  
    auto IC = ImageCenter::get_instance();
    auto DC = DataCenter::get_instance();
    auto bg = IC->get("./assets/image/scene/shop.png");

    //background
    al_draw_bitmap(bg, 0, 0, 0);

    //exit
    bool exitHover = Point(1100, 10).overlap(DC->mouse, 40);
    auto exit = IC->get(getButtonImage("./assets/image/littleStuff/exit.png", exitHover));
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
    case PUR_NOTI:{
        // Draw notification overlay on top
            auto bg_path = "./assets/image/scene/noti.png";
            al_draw_bitmap(IC->get(bg_path), 200, 50, 0);
            std::string str = (noti==SUCCESS)? "Purchess successful!": "OOPS! You're running short!";
            al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(255,255,255), 640, 360, ALLEGRO_ALIGN_CENTRE, str.c_str());
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
