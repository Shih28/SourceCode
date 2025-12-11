#include "Farm.h"
#include "../Game.h"
#include "../Utils.h"
#include "../facilities/Facility.h"
#include "../Player.h"
#include "../algif5/algif.h"
#include "../data/ImageCenter.h"
#include "../data/DataCenter.h"
#include "../shapes/Rectangle.h"
#include "../Food.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_primitives.h"
#include "../data/FontCenter.h"

const std::pair<int, int> MONS_POS[4] ={
    {100, 300},
    {400, 300},
    {700, 300},
    {1100, 300}
};

const std::pair<int, int> MONS_POS_FEED_MENU [] = {
    {300, 200},
    {800, 200}
};
const int WIDTH = 80;
const int HEIGHT = 150;

const std::pair<int, int> FOOD_DISPLAY_POS[] = {
    {320, 300}, 
    {960, 300},
    {320, 500},
    {960, 500}
};

const std::pair<int, int> FEED_BUTTON[] = {
    {300, 500},
    {800, 500}
};

const int BAR_LENGTH = 200;
const int BAR_HEIGHT = 15;
const std::string BAR_IMG[] = {
    "./assets/image/littleStuff/blue_bar.png",
    "./assets/image/littleStuff/yellow_bar.png"
};


const int MAX_ELE_PER_PAGE = 4;

int Farm::page = 0;

void Farm::init(){
    page = 0;
    Player* pl = Player::getPlayer();
    Facility &acessFac = pl->getFacilities()[pl->getAcessID()];

    if(acessFac.getType()==Facility::UNDETERMINE){
        state = LAND_SETTING;
    }else if(acessFac.getType()==Facility::FARM){
        state = FARM_MAIN;
    }else{
        state = HABITAT_MAIN;
    }
    
}

void Farm::update(){
    
    Player* pl = Player::getPlayer();
    Facility &acessFac = pl->getFacilities()[pl->getAcessID()];
    auto DC = DataCenter::get_instance();

    //monster updates
    for(auto &m: pl->getMonsters()){
        // debug_log("monster_update%d: %d %d\n", m.getID(), m.getPlacing(), m.getStatus());
        m.update();
    }
    
    switch (state){
        case LAND_SETTING:{
            page = 0;
            int &coin = pl->getCoin();
            //farm
           if(Rectangle(50, 350, 200, 500).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
            if(coin < 1000){
                noti = PUR_FAIL;
            }else{
                coin -= 1000;
                acessFac.setType(Facility::TYPE_F::FARM);
                acessFac.setStatus(Facility::STATUS_F::IDLE);
                noti = PUR_SUC;
            }
            pre_state = LAND_SETTING;
            state = PUR_NOTI;
           }//fire
           else if(Rectangle(250, 350, 400, 500).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                if(coin < 1000){
                    noti = PUR_FAIL;
                }else{
                    coin -=1000;
                    acessFac.setType(Facility::TYPE_F::FIRE_HABITAT);    
                    acessFac.setStatus(Facility::STATUS_F::IDLE);
                    noti = PUR_SUC;
                }
                pre_state = LAND_SETTING;
                state = PUR_NOTI;
           }//water
           else if(Rectangle(500, 350, 650, 500).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                if(coin < 1000){
                    noti = PUR_FAIL;
                }else{
                    coin -=1000;
                    acessFac.setType(Facility::TYPE_F::WATER_HABITAT);    
                    acessFac.setStatus(Facility::STATUS_F::IDLE);
                    noti = PUR_SUC;
                }
                pre_state = LAND_SETTING;
                state = PUR_NOTI;
           }//wind
           else if(Rectangle(750, 350, 900, 500).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                if(coin < 1500){
                    noti = PUR_FAIL;
                }else{
                    coin-=1500;
                    acessFac.setType(Facility::TYPE_F::WIND_HABITAT);    
                    acessFac.setStatus(Facility::STATUS_F::IDLE);
                    noti = PUR_SUC;
                }
                pre_state = LAND_SETTING;
                state = PUR_NOTI;
           }//lightning
           else if(Rectangle(1000, 350, 1150, 500).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                if(coin < 2000){
                    noti = PUR_FAIL;
                }else{
                    coin-=2000;
                    acessFac.setType(Facility::TYPE_F::LIGHTNING_HABITAT);    
                    acessFac.setStatus(Facility::STATUS_F::IDLE);
                    noti = PUR_SUC;
                }
                pre_state = LAND_SETTING;
                state = PUR_NOTI;
           }//exit
           else if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                pl->setrequest(Game::STATE::MENU);
           }
            break;
        }
        case HABITAT_MAIN:{
            for(int i=0; i<2; i++){
                if(acessFac.getHaveMonsters(i)){
                    int idx = acessFac.getMonsterIndex(i);
                    if(idx >= 0 && idx < (int)pl->getMonsters().size()){
                        pl->getMonsters()[idx].setPlacing(Monster::PLACE_M::FEEDING);
                    }
                }
           }
           int &berries = pl->getBer();

            //exit
           if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                pl->setrequest(Game::STATE::MENU);

                // mark facility monsters as HABITAT by modifying the player-owned monsters
                for(int i=0; i<2; i++){
                    if(acessFac.getHaveMonsters(i)){
                        int idx = acessFac.getMonsterIndex(i);
                        if(idx >= 0 && idx < (int)pl->getMonsters().size()){
                            pl->getMonsters()[idx].setPlacing(Monster::PLACE_M::HABITAT);
                        }
                    }
                }
           }//add monsters if slot empty
           else if(Point(500, 400).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                if(!acessFac.getHaveMonsters(0)){
                    pl->setAcessFacIdx(0);
                    state = HABITAT_MONSTERS;
                }
           }
           //add monsters if slot empty
           else if(Point(800, 400).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                if(!acessFac.getHaveMonsters(1)){
                    pl->setAcessFacIdx(1);
                    state = HABITAT_MONSTERS;
                }
           }//feed
           else if(Rectangle(FEED_BUTTON[0].first, FEED_BUTTON[0].second, FEED_BUTTON[0].first+150, FEED_BUTTON[0].second+80).overlap(DC->mouse)
                    && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                if(acessFac.getHaveMonsters(0)){
                    if(berries>=100){
                        debug_log("FEED!");
                        berries -= 100;
                        pl->getMonsters()[acessFac.getMonsterIndex(0)].Feed();
                    }else{
                        noti = FEED_FAIL;
                        pre_state = HABITAT_MAIN;
                        state = PUR_NOTI;
                    }
                }
            }//feed
            else if(Rectangle(FEED_BUTTON[1].first, FEED_BUTTON[1].second, FEED_BUTTON[1].first+150, FEED_BUTTON[1].second+80).overlap(DC->mouse)
                    && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                if(acessFac.getHaveMonsters(1)){
                    
                    if(berries>=100){
                        debug_log("FEED!");
                        berries -= 100;
                        pl->getMonsters()[acessFac.getMonsterIndex(1)].Feed();
                    }else{
                        noti = FEED_FAIL;
                        pre_state = HABITAT_MAIN;
                        state = PUR_NOTI;
                    }
                }
            }


            
            break;
        }
        case HABITAT_MONSTERS: {

            updateMonstersInDisplay();
            //exit
            if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                pl->setrequest(Game::STATE::MENU);

                for(int i=0; i<2; i++){
                    if(acessFac.getHaveMonsters(i)){
                        int idx = acessFac.getMonsterIndex(i);
                        if(idx >= 0 && idx < (int)pl->getMonsters().size()){
                            pl->getMonsters()[idx].setPlacing(Monster::PLACE_M::HABITAT);
                        }
                    }
                }
            }//more monsters button
            else if(Rectangle(500, 450, 700, 600).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                pl->setrequest(Game::STATE::STORE);
            }

            //monster selection
            for(int i=0; i<MAX_NUM; i++){
                if(Rectangle(MONS_POS[i].first, MONS_POS[i].second, 
                MONS_POS[i].first+WIDTH, MONS_POS[i].second+HEIGHT).overlap(DC->mouse) 
                && DC->mouse_state[1] && !DC->prev_mouse_state[1]){

                    int libIdx = monster_in_display_idx[i];
                    auto &owned = pl->getMonsters();

                    if(libIdx < 0 || libIdx >= (int)owned.size()){
                        continue;
                    }

                    int slot = pl->getAcessFacIdx();
                    acessFac.setMonsterIndex(slot, libIdx); // link facility slot to player's monster index
                    owned[libIdx].setFacilityRec(acessFac.getHitbox());
                    owned[libIdx].setPosMenu((int)acessFac.getHitbox().leftmost(), (int)acessFac.getHitbox().upmost());
                    owned[libIdx].setPosFeed(MONS_POS_FEED_MENU[slot].first, MONS_POS_FEED_MENU[slot].second);
                    acessFac.setStatus(Facility::STATUS_F::WORKING);
                    al_start_timer(acessFac.getTimer());
                    debug_log("acess monster %d\n", libIdx);
                     state = HABITAT_MAIN;
                }
           }
            break;
        }
        case FARM_MAIN:{
            //exit
            if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                pl->setrequest(Game::STATE::MENU);
            } //level up button
            else if(Rectangle(500, 450, 700, 600).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1] && acessFac.getLevel()<3){
                auto & coin = pl->getCoin();
                if(acessFac.getLevel()==1){
                    if(coin < 1500){
                        noti = LEVEL_UP_FAIL;
                    }else{
                        coin -= 1500;
                        acessFac.getLevel()++;
                        noti = LEVEL_UP_SUC;
                    }
                }else{
                    if(coin < 3000){
                        noti = LEVEL_UP_FAIL;
                    }else{
                        coin -= 3000;
                        acessFac.getLevel()++;
                        noti = LEVEL_UP_SUC;
                    }
                }
                pre_state = FARM_MAIN;
                state = PUR_NOTI;
            }
            //food buttons
            for(int i=0; i<MAX_ELE_PER_PAGE; i++){

                if(Rectangle(FOOD_DISPLAY_POS[i].first, FOOD_DISPLAY_POS[i].second, 
                    FOOD_DISPLAY_POS[i].first+Food::width, FOOD_DISPLAY_POS[i].second+Food::length).overlap(DC->mouse)
                    && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                        auto &lib = pl->getAllFoods();
                        auto &coin = pl->getCoin();
                        auto price = lib[static_cast<Food::TYPE_F>(i)].getPrice();

                        if(coin < price){
                            noti = PUR_FAIL;
                        }else{
                            coin -= price;
                            acessFac.setFood(lib[static_cast<Food::TYPE_F>(i)].getType());
                            al_start_timer(acessFac.getTimer());
                            acessFac.setStatus(Facility::STATUS_F::WORKING);

                            noti = PUR_SUC;
                        }
                        pre_state = FARM_MAIN;
                        state = PUR_NOTI;
                    }
            }
            
            //set food's display positions
            for(int i=0; i<MAX_ELE_PER_PAGE; i++){
                auto &lib = pl->getAllFoods();
                lib[static_cast<Food::TYPE_F>(i)].setPos(FOOD_DISPLAY_POS[i].first, FOOD_DISPLAY_POS[i].second);
            }
            break;
        }
        case PUR_NOTI:{
            if(pur_noti_cnt<=60){
                pur_noti_cnt++;
            }else{
                pur_noti_cnt = 0;
                // If we came from LAND_SETTING and made a successful purchase,
                // need to transition to the appropriate state based on facility type
                if(pre_state == LAND_SETTING && noti == PUR_SUC){
                    if(acessFac.getType() == Facility::FARM){
                        state = FARM_MAIN;
                    }else{
                        state = HABITAT_MAIN;
                    }
                }else if(pre_state==FARM_MAIN){
                    pl->setrequest(Game::STATE::MENU);
                }else{
                    state = pre_state;
                }
            }
            break;
        }
        default: break;
    }
}

void Farm::draw(){
    Player* pl = Player::getPlayer();
    Facility &acessFac = pl->getFacilities()[pl->getAcessID()];
    auto IC = ImageCenter::get_instance();
    std::string bg_path;
    
    switch (state){
        case LAND_SETTING:{

            LAND_SETTING_GOTO:

            bg_path = "./assets/image/scene/buildings.png";

            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);
            //farm
            auto farm = IC->get("./assets/image/store/farm.png");
            al_draw_bitmap(farm, 50, 350, 0);

            //habitats
            auto fire = IC->get("./assets/image/store/fire_hab.png");
            auto water = IC->get("./assets/image/store/water_hab.png");
            auto wind = IC->get("./assets/image/store/wind_hab.png");
            auto lightning = IC->get("./assets/image/store/lightning_hab.png");

            al_draw_bitmap(fire, 250, 350, 0);
            al_draw_bitmap(water, 500, 350, 0);
            al_draw_bitmap(wind, 750, 350, 0);
            al_draw_bitmap(lightning, 1000, 350, 0);

            //exit
            auto exit = IC->get("./assets/image/littleStuff/exit.png");
            
            al_draw_bitmap(exit, 1100, 10, 0);


            //hitboxes
            al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2); //exit
            al_draw_rectangle(50, 350, 200, 500, al_map_rgb(255,0,0), 2); //farm 
            al_draw_rectangle(250, 350, 400, 500, al_map_rgb(255,0,0), 2); //fire
            al_draw_rectangle(500, 350, 650, 500, al_map_rgb(255,0,0), 2); //water
            al_draw_rectangle(750, 350, 900, 500, al_map_rgb(255,0,0), 2); //wind
            al_draw_rectangle(1000, 350, 1150, 500, al_map_rgb(255,0,0), 2); //lightning
            

            break;
        }
        case HABITAT_MAIN:{

            bg_path = "./assets/image/scene/feed.png";
            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);

            //exit
            auto exit = IC->get("./assets/image/littleStuff/exit.png");
            al_draw_bitmap(exit, 1100, 10, 0);

            //add button
            auto add = IC->get("./assets/image/littleStuff/add.png");
            if(!acessFac.getHaveMonsters(0)) al_draw_bitmap(add, 500, 400, 0);
            if(!acessFac.getHaveMonsters(1)) al_draw_bitmap(add, 800, 400, 0);
            
            //hitboxes
            al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);
            al_draw_circle(500, 400, 40, al_map_rgb(255,0,0), 2);
            al_draw_circle(800, 400, 40, al_map_rgb(255,0,0), 2);

            //monster in hab
            for(int i=0; i<2; i++){
                if(acessFac.getHaveMonsters(i)){
                    int idx = acessFac.getMonsterIndex(i);
                    if(idx >= 0 && idx < (int)pl->getMonsters().size()){
                        pl->getMonsters()[idx].draw();
                    }
                }
            }

            //berries
            auto berry = IC->get("./assets/image/littleStuff/berry_bar.png");
            al_draw_bitmap(berry, 50, 5, 0);
            std::string b = std::to_string(pl->getBer());
            al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(0,0,0), 75, 15, ALLEGRO_ALIGN_CENTRE, b.c_str());

            for(int i=0; i<2; i++){
                if(acessFac.getHaveMonsters(i)){
                    auto feedBut = IC->get("./assets/image/littleStuff/b100.png");
                    al_draw_bitmap(feedBut, FEED_BUTTON[i].first, FEED_BUTTON[i].second, 0);
                    al_draw_rectangle(FEED_BUTTON[i].first, FEED_BUTTON[i].second, FEED_BUTTON[i].first+150, FEED_BUTTON[i].second+80, al_map_rgb(255,0,0), 2);
                }
                
            }

            for(int i=0; i<2; i++){
                if(acessFac.getHaveMonsters(i)){
                    auto blue_bar = IC->get(BAR_IMG[0]);
                    auto yellow_bar = IC->get(BAR_IMG[1]);
                    
                    double scale = ((double)pl->getMonsters()[acessFac.getMonsterIndex(i)].getExp())/((double)Monster::EXP);
                    double w = scale*BAR_LENGTH;

                    // debug_log("w: %lf\n", w);
                    al_draw_bitmap(blue_bar, FEED_BUTTON[i].first, FEED_BUTTON[i].second-80, 0);
                    al_draw_bitmap_region(yellow_bar, 0, 0, w, 100, FEED_BUTTON[i].first, FEED_BUTTON[i].second-80, 0);

                    std::string str = std::to_string(pl->getMonsters()[acessFac.getMonsterIndex(i)].getExp());
                    str = str + "/1000";
                    al_draw_text(FontCenter::get_instance()->caviar_dreams[24], al_map_rgb(255,255,255),FEED_BUTTON[i].first+50, FEED_BUTTON[i].second-100, ALLEGRO_ALIGN_CENTRE, str.c_str());
                }
            }
            
            break;
        }
        case HABITAT_MONSTERS:{
            bg_path = "./assets/image/scene/feed.png";
            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);
            
            //exit
            auto exit = IC->get("./assets/image/littleStuff/exit.png");
            al_draw_bitmap(exit, 1100, 10, 0);

            //selection tab
            auto sel = IC->get("./assets/image/scene/selection.png");
            al_draw_bitmap(sel, 100, 50, 0);

            //owned monsters
            debug_log("owned: %d\n", pl->getMonsters().size());
            int i=0;
            for(auto &m: pl->getMonsters()){
                if(m.getPlacing()==Monster::PLACE_M::NONE && match(m, acessFac)){
                    al_draw_bitmap(m.getImgInPfp(), MONS_POS[i].first, MONS_POS[i].second, 0);
                    al_draw_rectangle(MONS_POS[i].first, MONS_POS[i].second, MONS_POS[i].first+WIDTH, MONS_POS[i].second+HEIGHT, al_map_rgb(255,0,0), 2);
                    i++;
                }
            }

            //more monsters button
            auto more = IC->get("./assets/image/littleStuff/more.png");
            al_draw_bitmap(more, 500, 450, 0);

            //hitboxes
            al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);
            al_draw_rectangle(500, 450, 700, 600, al_map_rgb(255,0,0), 2);
            break;
        }
        case FARM_MAIN:{

            FARM_MAIN_GOTO:

            bg_path = "./assets/image/scene/food.png";
            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);

            //exit
            auto exit = IC->get("./assets/image/littleStuff/exit.png");
            al_draw_bitmap(exit, 1100, 10, 0);

            // level up button
            if(acessFac.getLevel()<3){
            auto level = IC->get("./assets/image/littleStuff/level_up.png");
            al_draw_bitmap(level, 500, 450, 0);
            }

            //foods
            for(int i=0; i<MAX_ELE_PER_PAGE; i++){
                auto &lib = pl->getAllFoods();
                lib[static_cast<Food::TYPE_F>(i)].draw();
            }

            //hitboxes
            al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);
            al_draw_rectangle(500, 450, 700, 600, al_map_rgb(255,0,0), 2);
            for(int i=0; i<MAX_ELE_PER_PAGE; i++){
                al_draw_rectangle(FOOD_DISPLAY_POS[i].first, FOOD_DISPLAY_POS[i].second, FOOD_DISPLAY_POS[i].first+Food::width, FOOD_DISPLAY_POS[i].second+Food::length, al_map_rgb(255,0,0), 2);
            }
            break;
        }
        case PUR_NOTI:{
            // Draw the background based on the previous state
            if(pre_state==FARM_MAIN){
                bg_path = "./assets/image/scene/food.png";
                al_draw_bitmap(IC->get(bg_path), 0, 0, 0);
                
                auto exit = IC->get("./assets/image/littleStuff/exit.png");
                al_draw_bitmap(exit, 1100, 10, 0);
                
                if(acessFac.getLevel()<3){
                    auto level = IC->get("./assets/image/littleStuff/level_up.png");
                    al_draw_bitmap(level, 500, 450, 0);
                }
                
                for(int i=0; i<MAX_ELE_PER_PAGE; i++){
                    auto &lib = pl->getAllFoods();
                    lib[static_cast<Food::TYPE_F>(i)].draw();
                }
            }else if(pre_state==LAND_SETTING){
                bg_path = "./assets/image/scene/buildings.png";
                al_draw_bitmap(IC->get(bg_path), 0, 0, 0);
                
                auto farm = IC->get("./assets/image/store/farm.png");
                al_draw_bitmap(farm, 50, 350, 0);
                
                auto fire = IC->get("./assets/image/store/fire_hab.png");
                auto water = IC->get("./assets/image/store/water_hab.png");
                auto wind = IC->get("./assets/image/store/wind_hab.png");
                auto lightning = IC->get("./assets/image/store/lightning_hab.png");
                
                al_draw_bitmap(fire, 250, 350, 0);
                al_draw_bitmap(water, 500, 350, 0);
                al_draw_bitmap(wind, 750, 350, 0);
                al_draw_bitmap(lightning, 1000, 350, 0);
                
                auto exit = IC->get("./assets/image/littleStuff/exit.png");
                al_draw_bitmap(exit, 1100, 10, 0);
            }else if(pre_state==HABITAT_MAIN){
                bg_path = "./assets/image/scene/feed.png";
                al_draw_bitmap(IC->get(bg_path), 0, 0, 0);
                
                auto exit = IC->get("./assets/image/littleStuff/exit.png");
                al_draw_bitmap(exit, 1100, 10, 0);
                
                auto add = IC->get("./assets/image/littleStuff/add.png");
                al_draw_bitmap(add, 500, 400, 0);
                al_draw_bitmap(add, 800, 400, 0);
                
                for(int i=0; i<2; i++){
                    if(acessFac.getHaveMonsters(i)){
                        int idx = acessFac.getMonsterIndex(i);
                        if(idx >= 0 && idx < (int)pl->getMonsters().size()){
                            pl->getMonsters()[idx].draw();
                        }
                    }
                }
            }

            // Draw notification overlay on top
            bg_path = "./assets/image/scene/noti.png";
            al_draw_bitmap(IC->get(bg_path), 200, 50, 0);
            std::string str = 
                (noti==LEVEL_UP_SUC)? "Successfully leveled up!":
                (noti==LEVEL_UP_FAIL)? "OOPS! You're running short, maybe next time?":
                (noti==PUR_SUC && pre_state==FARM_MAIN)? "Planted!": 
                (noti==PUR_SUC)? "Purchase successful!": 
                (noti==FEED_FAIL)? "OOPS! Not enough berries!":
                "OOPS! You're running short, maybe next time?";
            al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(255,255,255), 640, 360, ALLEGRO_ALIGN_CENTRE, str.c_str());
            break;
        }
    }

    
}

void Farm::end(){

}

// Updates the monster_in_display_idx array to hold indices of player's monsters
// that are not currently placed and match the accessed facility, for display purposes.
void Farm::updateMonstersInDisplay(){
    auto *pl = Player::getPlayer();
    auto &lib = pl->getMonsters();
    auto &acessFac = pl->getFacilities()[pl->getAcessID()];
    // reset indices
    for(int k=0; k<MAX_NUM; ++k) monster_in_display_idx[k] = -1;

    int j = 0;
    for(int i = 0; i < lib.size(); ++i){
        if(j == MAX_NUM) break;

        if(lib[i].getPlacing()==Monster::PLACE_M::NONE && match(lib[i], acessFac)){
            monster_in_display_idx[j] = i;
            j++;
        }   
    }
}

void Farm::updateFoodInDisplay(){
    auto *pl = Player::getPlayer();
    auto &lib = pl->getAllFoods();

    for(int i=0; i<Food::MAX_TYPE; i++) food_in_display_idx[i]=-1;

    int i=0;
    for(int j=page*MAX_ELE_PER_PAGE; j<lib.size() && i<MAX_ELE_PER_PAGE; j++){
        food_in_display_idx[i] = j;
        i++;
    }
}