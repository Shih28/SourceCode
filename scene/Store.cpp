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
const int MAX_NUM = 8;

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
const int SCREEN_HEIGHT = 720;
const int WIDTH = 213;
const int HEIGHT = 230;

// Calculate evenly spaced positions for 4x2 grid (4 monsters per row, 2 rows)
const int COLS = 4;
const int ROWS = 2;
const int HORIZONTAL_SPACING = 250;  // Space between each monster horizontally
const int VERTICAL_SPACING = 260;     // Space between rows
const int GRID_WIDTH = (COLS - 1) * HORIZONTAL_SPACING + WIDTH;
const int GRID_HEIGHT = (ROWS - 1) * VERTICAL_SPACING + HEIGHT;
const int START_X = (SCREEN_WIDTH - GRID_WIDTH) / 2;
const int START_Y = (SCREEN_HEIGHT - GRID_HEIGHT) / 2;

// Generate 4x2 grid positions using math
const std::pair<int, int> MONS_POS[] = {
    // Row 1
    {START_X + 0 * HORIZONTAL_SPACING, START_Y + 0 * VERTICAL_SPACING},
    {START_X + 1 * HORIZONTAL_SPACING, START_Y + 0 * VERTICAL_SPACING},
    {START_X + 2 * HORIZONTAL_SPACING, START_Y + 0 * VERTICAL_SPACING},
    {START_X + 3 * HORIZONTAL_SPACING, START_Y + 0 * VERTICAL_SPACING},
    
    // Row 2
    {START_X + 0 * HORIZONTAL_SPACING, START_Y + 1 * VERTICAL_SPACING},
    {START_X + 1 * HORIZONTAL_SPACING, START_Y + 1 * VERTICAL_SPACING},
    {START_X + 2 * HORIZONTAL_SPACING, START_Y + 1 * VERTICAL_SPACING},
    {START_X + 3 * HORIZONTAL_SPACING, START_Y + 1 * VERTICAL_SPACING}
};

void Store::init(){
    state = ALL;
    noti_cnt=0;
    page=0;
}

void Store::update(){
    auto pl = Player::getPlayer();
    auto DC = DataCenter::get_instance();
    
    //exit
    if(Point(1142, 47).overlap(DC->mouse, 38) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
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

                
                auto &mask = lib[i*2];
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
    bool exitHover = Point(1142, 47).overlap(DC->mouse, 38);
    auto exit = IC->get(getButtonImage("./assets/image/littleStuff/exit.png", exitHover));
    al_draw_bitmap(exit, 1100, 10, 0);

    //monsters
    switch (state){
    case ALL:{
        // debug_log("lib size: %d\n", lib.size());
        // draw up to MAX_NUM items (4x2 grid = 8 monsters)
        
        for(int i = 0; i < MAX_NUM && i < lib.size()/2; i++){
            
            // Check if mouse is hovering over this monster
            bool monsterHover = Rectangle(MONS_POS[i].first, MONS_POS[i].second,
                MONS_POS[i].first+WIDTH, MONS_POS[i].second+HEIGHT).overlap(DC->mouse);
            
            auto img = monsterHover ? lib[i*2].getImgInStoreHover() : lib[i*2].getImgInStore();
            if(img) al_draw_bitmap(img, MONS_POS[i].first, MONS_POS[i].second, 0);

            //coins
            auto coin = IC->get("./assets/image/littleStuff/coin_bar.png");
            al_draw_bitmap(coin, 50, 5, 0);
            std::string c = std::to_string(pl->getCoin());
            al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(0,0,0), 200, 18, ALLEGRO_ALIGN_CENTRE, c.c_str());

        }
        break;
    }
    case PUR_NOTI:{
        // Draw notification overlay on top
            for(int i = 0; i < MAX_NUM && i < lib.size()/2; i++){
            
                auto img = lib[i*2].getImgInStore();
                if(img) al_draw_bitmap(img, MONS_POS[i].first, MONS_POS[i].second, 0);

                //coins
                auto coin = IC->get("./assets/image/littleStuff/coin_bar.png");
                al_draw_bitmap(coin, 50, 5, 0);
                std::string c = std::to_string(pl->getCoin());
                al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(0,0,0), 200, 18, ALLEGRO_ALIGN_CENTRE, c.c_str());

            }


            auto bg_path = "./assets/image/scene/noti.png";
            al_draw_bitmap(IC->get(bg_path), 200, 50, 0);

            std::string str = (noti==SUCCESS)? "Purchess successful!": "OOPS! You're running short!";
            al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(255,255,255), 640, 360, ALLEGRO_ALIGN_CENTRE, str.c_str());
        break;
    }
    }

    //hitboxes
    // al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);

}

void Store::end(){

}
