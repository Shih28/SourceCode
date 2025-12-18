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

int Farm::page = 0;

// Screen and element dimensions
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int WIDTH = 213;
const int HEIGHT = 234;

// Monster selection positions (3 monsters in a row, horizontally centered)
const int MONS_SPACING = 250;
const int MONS_VERTICAL_SPACING = 260;
const int MONS_GRID_WIDTH = 2 * MONS_SPACING + WIDTH;
const int MONS_START_X = (SCREEN_WIDTH - MONS_GRID_WIDTH) / 2 + 10;
const int MONS_Y = 100;


const std::pair<int, int> MONS_POS[6] = {
    {MONS_START_X + 0 * MONS_SPACING, MONS_Y},
    {MONS_START_X + 1 * MONS_SPACING, MONS_Y},
    {MONS_START_X + 2 * MONS_SPACING, MONS_Y},
    
    {MONS_START_X + 0 * MONS_SPACING, MONS_Y+MONS_VERTICAL_SPACING},
    {MONS_START_X + 1 * MONS_SPACING, MONS_Y+MONS_VERTICAL_SPACING},
    {MONS_START_X + 2 * MONS_SPACING, MONS_Y+MONS_VERTICAL_SPACING}

};

// Feeding menu positions (2 monsters, evenly spaced)
const int FEED_SPACING = 500;
const int FEED_START_X = (SCREEN_WIDTH - FEED_SPACING) / 2;
const int FEED_Y = 300;

// const std::pair<int, int> MONS_POS_FEED_MENU[] = {
//     {FEED_START_X-190, FEED_Y},
//     {FEED_START_X-15 + FEED_SPACING, FEED_Y}
// };

const std::pair<int, int> MONS_POS_FEED_MENU[] = {
    {150, 200},
    {753,  200}
};

// Food display positions (2x2 grid, centered)
const int FOOD_H_SPACING = 480;  // Horizontal spacing between columns
const int FOOD_V_SPACING = 280;  // Vertical spacing between rows
const int FOOD_START_X = 265;
const int FOOD_START_Y = 120;

const std::pair<int, int> FOOD_DISPLAY_POS[] = {
    {FOOD_START_X, FOOD_START_Y},                           // Top-left
    {FOOD_START_X + FOOD_H_SPACING, FOOD_START_Y},         // Top-right
    {FOOD_START_X, FOOD_START_Y + FOOD_V_SPACING},         // Bottom-left
    {FOOD_START_X + FOOD_H_SPACING, FOOD_START_Y + FOOD_V_SPACING}  // Bottom-right
};

// Feed button positions (aligned with feeding positions)
const std::pair<int, int> FEED_BUTTON[] = {
    {FEED_START_X-150, 650},
    {FEED_START_X + FEED_SPACING, 650}
};

const int BAR_LENGTH = 200;
const int BAR_HEIGHT = 15;
const std::string BAR_IMG[] = {
    "./assets/image/littleStuff/blue_bar.png",
    "./assets/image/littleStuff/yellow_bar.png"
};


const int MAX_ELE_PER_PAGE = 4;

// Helper function to get button image with hover effect
// If hovering is true, attempts to load "name2.png", otherwise loads "name.png"
static std::string getButtonImage(const std::string& basePath, bool hovering) {
    if (!hovering) return basePath;
    
    // Find the .png extension and insert "2" before it
    size_t dotPos = basePath.rfind(".png");
    if (dotPos != std::string::npos) {
        return basePath.substr(0, dotPos) + "2.png";
    }
    return basePath;
}

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
        m.update();
    }
    
    switch (state){
        case LAND_SETTING:{
            page = 0;
            int &coin = pl->getCoin();
            //farm
           if(Rectangle(300, 100, 570, 350).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
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
           else if(Rectangle(100, 400, 370, 610).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
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
           else if(Rectangle(500, 400, 770, 610).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
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
           else if(Rectangle(700, 100, 970, 350).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
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
           else if(Rectangle(900, 400, 1170, 610).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
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
           else if(Point(1142, 47).overlap(DC->mouse, 38) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
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
           if(Point(1142, 47).overlap(DC->mouse, 38) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
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
           else if(Point(340, 420).overlap(DC->mouse, 62) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                if(!acessFac.getHaveMonsters(0)){
                    pl->setAcessFacIdx(0);
                    state = HABITAT_MONSTERS;
                }
           }
           //add monsters if slot empty
           else if(Point(970, 420).overlap(DC->mouse, 62) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
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
            if(Point(1142, 47).overlap(DC->mouse, 38) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
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
            else if(Rectangle(540, 630, 750, 720).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
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
            if(Point(1142, 47).overlap(DC->mouse, 38) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                pl->setrequest(Game::STATE::MENU);
            } //level up button
            else if(Rectangle(550, 630, 785, 700).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1] && acessFac.getLevel()<3){
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
            if(pur_noti_cnt<=30){
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
    auto DC = DataCenter::get_instance();
    std::string bg_path;
    
    switch (state){
        case LAND_SETTING:{

            bg_path = "./assets/image/scene/buildings.png";

            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);

            //farm
            bool farmHover = Rectangle(300, 100, 570, 350).overlap(DC->mouse);
            auto farm = IC->get(getButtonImage("./assets/image/store/farm.png", farmHover));
            
            //habitats
            bool fireHover = Rectangle(100, 400, 370, 610).overlap(DC->mouse);
            auto fire = IC->get(getButtonImage("./assets/image/store/fire_hab.png", fireHover));
            
            bool waterHover = Rectangle(500, 400, 770, 610).overlap(DC->mouse);
            auto water = IC->get(getButtonImage("./assets/image/store/water_hab.png", waterHover));
            
            bool windHover = Rectangle(700, 100, 970, 350).overlap(DC->mouse);
            auto wind = IC->get(getButtonImage("./assets/image/store/wind_hab.png", windHover));
            
            bool lightningHover = Rectangle(900, 400, 1170, 610).overlap(DC->mouse);
            auto lightning = IC->get(getButtonImage("./assets/image/store/lightning_hab.png", lightningHover));

            al_draw_bitmap(fire, 100, 400, 0);
            al_draw_bitmap(water, 500, 400, 0);
            al_draw_bitmap(lightning, 900, 400, 0);

            al_draw_bitmap(wind, 700, 100, 0);
            al_draw_bitmap(farm, 300, 100, 0);

            //exit
            bool exitHover = Point(1142, 47).overlap(DC->mouse, 38);
            auto exit = IC->get(getButtonImage("./assets/image/littleStuff/exit.png", exitHover));
            al_draw_bitmap(exit, 1100, 10, 0);

            //coins
            auto coin = IC->get("./assets/image/littleStuff/coin_bar.png");
            al_draw_bitmap(coin, 50, 5, 0);
            std::string c = std::to_string(pl->getCoin());
            al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(0,0,0), 200, 18, ALLEGRO_ALIGN_CENTRE, c.c_str());



            //hitboxes
            // al_draw_circle(1142, 47, 38, al_map_rgb(255,0,0), 2); //exit
            // al_draw_rectangle(700, 100, 970, 350, al_map_rgb(255,0,0), 2); //wind
            // al_draw_rectangle(300, 100, 570, 350, al_map_rgb(255,0,0), 2); //farm 

            // al_draw_rectangle(100, 400, 370, 610, al_map_rgb(255,0,0), 2); //fire
            // al_draw_rectangle(500, 400, 770, 610, al_map_rgb(255,0,0), 2); //water
            // al_draw_rectangle(900, 400, 1170, 610, al_map_rgb(255,0,0), 2); //lightning
            

            break;
        }
        case HABITAT_MAIN:{

            bg_path = "./assets/image/scene/feed.png";
            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);

            //exit
            bool exitHover = Point(1142, 47).overlap(DC->mouse, 38);
            auto exit = IC->get(getButtonImage("./assets/image/littleStuff/exit.png", exitHover));
            al_draw_bitmap(exit, 1100, 10, 0);

            //add button
            if(!acessFac.getHaveMonsters(0)) {
                bool add1Hover = Point(340, 420).overlap(DC->mouse, 62);
                auto add1 = IC->get(getButtonImage("./assets/image/littleStuff/add.png", add1Hover));
                al_draw_bitmap(add1, 270, 350, 0);
            }
            if(!acessFac.getHaveMonsters(1)) {
                bool add2Hover = Point(970, 420).overlap(DC->mouse, 62);
                auto add2 = IC->get(getButtonImage("./assets/image/littleStuff/add.png", add2Hover));
                al_draw_bitmap(add2, 900, 350, 0);
            }
            
            //hitboxes
            // al_draw_circle(340, 420, 62, al_map_rgb(255,0,0), 2);
            // al_draw_circle(970, 420, 62, al_map_rgb(255,0,0), 2);

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
            al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(0,0,0), 200, 18, ALLEGRO_ALIGN_CENTRE, b.c_str());

            //feed
            for(int i=0; i<2; i++){
                if(acessFac.getHaveMonsters(i) && pl->getMonsters()[acessFac.getMonsterIndex(i)].getLevel()<=2){
                    auto feedBut = IC->get("./assets/image/littleStuff/b100.png");
                    al_draw_bitmap(feedBut, FEED_BUTTON[i].first, FEED_BUTTON[i].second, 0);
                    
                    // al_draw_rectangle(FEED_BUTTON[i].first, FEED_BUTTON[i].second,
                        // FEED_BUTTON[i].first+200, FEED_BUTTON[i].second+60, al_map_rgb(255,0,0), 2);
                }
                
            }

            for(int i=0; i<2; i++){
                if(acessFac.getHaveMonsters(i)){
                    auto blue_bar = IC->get(BAR_IMG[0]);
                    auto yellow_bar = IC->get(BAR_IMG[1]);
                    
                    double scale = ((double)pl->getMonsters()[acessFac.getMonsterIndex(i)].getExp())/((double)Monster::EXP);
                    double w = scale*BAR_LENGTH;

                    al_draw_bitmap(blue_bar, FEED_BUTTON[i].first, FEED_BUTTON[i].second-40, 0);
                    al_draw_bitmap_region(yellow_bar, 0, 0, w, 100, FEED_BUTTON[i].first, FEED_BUTTON[i].second-40, 0);

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
            bool exitHover = Point(1142, 47).overlap(DC->mouse, 38);
            auto exit = IC->get(getButtonImage("./assets/image/littleStuff/exit.png", exitHover));
            al_draw_bitmap(exit, 1100, 10, 0);

            //selection tab
            auto sel = IC->get("./assets/image/scene/selection.png");
            al_draw_bitmap(sel, 250, 10, 0);

            //owned monsters
            debug_log("owned: %d\n", pl->getMonsters().size());
            int i=0;
            for(auto &m: pl->getMonsters()){
                if(m.getPlacing()==Monster::PLACE_M::NONE && match(m, acessFac)){
                    // Check if mouse is hovering over this monster
                    bool monsterHover = Rectangle(MONS_POS[i].first, MONS_POS[i].second,
                        MONS_POS[i].first+WIDTH, MONS_POS[i].second+HEIGHT).overlap(DC->mouse);
                    
                    auto img = monsterHover ? m.getImgInPfpHover() : m.getImgInPfp();
                    if(img) al_draw_bitmap(img, MONS_POS[i].first, MONS_POS[i].second, 0);

                    // al_draw_rectangle(MONS_POS[i].first, MONS_POS[i].second, 
                    //     MONS_POS[i].first+WIDTH, MONS_POS[i].second+HEIGHT, al_map_rgb(255,0,0), 2);
                    i++;
                }
            }

            //more monsters button
            bool moreHover = Rectangle(540, 630, 750, 720).overlap(DC->mouse);
            auto more = IC->get(getButtonImage("./assets/image/littleStuff/more.png", moreHover));
            al_draw_bitmap(more, 540, 630, 0);

            //hitboxes
            // al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);
            // al_draw_rectangle(540, 570, 750, 660, al_map_rgb(255,0,0), 2);
            break;
        }
        case FARM_MAIN:{

            bg_path = "./assets/image/scene/food.png";
            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);

            //exit
            bool exitHover = Point(1142, 47).overlap(DC->mouse, 38);
            auto exit = IC->get(getButtonImage("./assets/image/littleStuff/exit.png", exitHover));
            al_draw_bitmap(exit, 1100, 10, 0);

            // level up button
            if(acessFac.getLevel()<3){
                bool levelHover = Rectangle(550, 630, 785, 700).overlap(DC->mouse);
                auto level = IC->get(getButtonImage("./assets/image/littleStuff/level_up.png", levelHover));
                al_draw_bitmap(level, 550, 630, 0);
            }

            //foods

            for(int i=0; i<MAX_ELE_PER_PAGE; i++){
                auto &lib = pl->getAllFoods();
                bool foodhover = Rectangle(FOOD_DISPLAY_POS[i].first, FOOD_DISPLAY_POS[i].second, 
                    FOOD_DISPLAY_POS[i].first+Food::width, FOOD_DISPLAY_POS[i].second+Food::length).overlap(DC->mouse);

                lib[static_cast<Food::TYPE_F>(i)].draw(foodhover);
                if(acessFac.getLevel()>=2){
                    std::string img_path = acessFac.getLevel()==2? "./assets/image/littleStuff/x2" : "./assets/image/littleStuff/x3";
                    if(foodhover){
                        img_path = img_path + "2.png";
                    }else{
                        img_path = img_path + ".png";
                    }
                    auto img = IC->get(img_path);
                    al_draw_bitmap(img, FOOD_DISPLAY_POS[i].first+Food::width-40, FOOD_DISPLAY_POS[i].second, 0);
                }
            }

            //coins
            auto coin = IC->get("./assets/image/littleStuff/coin_bar.png");
            al_draw_bitmap(coin, 50, 5, 0);
            std::string c = std::to_string(pl->getCoin());
            al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(0,0,0), 200, 18, ALLEGRO_ALIGN_CENTRE, c.c_str());

            //hitboxes
            // al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);
            // al_draw_rectangle(550, 630, 785, 700, al_map_rgb(255,0,0), 2);
            // for(int i=0; i<MAX_ELE_PER_PAGE; i++){
            //     al_draw_rectangle(FOOD_DISPLAY_POS[i].first, FOOD_DISPLAY_POS[i].second, 
            //         FOOD_DISPLAY_POS[i].first+Food::width, FOOD_DISPLAY_POS[i].second+Food::length, al_map_rgb(255,0,0), 2);
            // }
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
                    al_draw_bitmap(level, 550, 630, 0);
                }
                
                for(int i=0; i<MAX_ELE_PER_PAGE; i++){
                    auto &lib = pl->getAllFoods();
                    lib[static_cast<Food::TYPE_F>(i)].draw(false);
                    if(acessFac.getLevel()>=2){
                        std::string img_path = acessFac.getLevel()==2? "./assets/image/littleStuff/x2.png" : "./assets/image/littleStuff/x3.png";
                        auto img = IC->get(img_path);
                        al_draw_bitmap(img, FOOD_DISPLAY_POS[i].first+Food::width-40, FOOD_DISPLAY_POS[i].second, 0);
                    }
                }
                
                auto coin = IC->get("./assets/image/littleStuff/coin_bar.png");
                al_draw_bitmap(coin, 50, 5, 0);
                std::string c = std::to_string(pl->getCoin());
                al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(0,0,0), 200, 18, ALLEGRO_ALIGN_CENTRE, c.c_str());
            }else if(pre_state==LAND_SETTING){
                bg_path = "./assets/image/scene/buildings.png";
                al_draw_bitmap(IC->get(bg_path), 0, 0, 0);
                
                auto farm = IC->get("./assets/image/store/farm.png");
                auto fire = IC->get("./assets/image/store/fire_hab.png");
                auto water = IC->get("./assets/image/store/water_hab.png");
                auto wind = IC->get("./assets/image/store/wind_hab.png");
                auto lightning = IC->get("./assets/image/store/lightning_hab.png");
                
                al_draw_bitmap(fire, 100, 400, 0);
                al_draw_bitmap(water, 500, 400, 0);
                al_draw_bitmap(lightning, 900, 400, 0);
                al_draw_bitmap(wind, 700, 100, 0);
                al_draw_bitmap(farm, 300, 100, 0);
                
                auto exit = IC->get("./assets/image/littleStuff/exit.png");
                al_draw_bitmap(exit, 1100, 10, 0);
                
                auto coin = IC->get("./assets/image/littleStuff/coin_bar.png");
                al_draw_bitmap(coin, 50, 5, 0);
                std::string c = std::to_string(pl->getCoin());
                al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(0,0,0), 200, 18, ALLEGRO_ALIGN_CENTRE, c.c_str());
            }else if(pre_state==HABITAT_MAIN){
                bg_path = "./assets/image/scene/feed.png";
                al_draw_bitmap(IC->get(bg_path), 0, 0, 0);
                
                auto exit = IC->get("./assets/image/littleStuff/exit.png");
                al_draw_bitmap(exit, 1100, 10, 0);
                
                if(!acessFac.getHaveMonsters(0)) {
                    auto add1 = IC->get("./assets/image/littleStuff/add.png");
                    al_draw_bitmap(add1, 270, 350, 0);
                }
                if(!acessFac.getHaveMonsters(1)) {
                    auto add2 = IC->get("./assets/image/littleStuff/add.png");
                    al_draw_bitmap(add2, 900, 350, 0);
                }
                
                for(int i=0; i<2; i++){
                    if(acessFac.getHaveMonsters(i)){
                        int idx = acessFac.getMonsterIndex(i);
                        if(idx >= 0 && idx < (int)pl->getMonsters().size()){
                            pl->getMonsters()[idx].draw();
                        }
                    }
                }
                
                auto berry = IC->get("./assets/image/littleStuff/berry_bar.png");
                al_draw_bitmap(berry, 50, 5, 0);
                std::string b = std::to_string(pl->getBer());
                al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(0,0,0), 200, 18, ALLEGRO_ALIGN_CENTRE, b.c_str());
                
                for(int i=0; i<2; i++){
                    if(acessFac.getHaveMonsters(i) && pl->getMonsters()[acessFac.getMonsterIndex(i)].getLevel()<=2){
                        auto feedBut = IC->get("./assets/image/littleStuff/b100.png");
                        al_draw_bitmap(feedBut, FEED_BUTTON[i].first, FEED_BUTTON[i].second, 0);
                    }
                }
                
                for(int i=0; i<2; i++){
                    if(acessFac.getHaveMonsters(i)){
                        auto blue_bar = IC->get(BAR_IMG[0]);
                        auto yellow_bar = IC->get(BAR_IMG[1]);
                        
                        double scale = ((double)pl->getMonsters()[acessFac.getMonsterIndex(i)].getExp())/((double)Monster::EXP);
                        double w = scale*BAR_LENGTH;

                        al_draw_bitmap(blue_bar, FEED_BUTTON[i].first, FEED_BUTTON[i].second-40, 0);
                        al_draw_bitmap_region(yellow_bar, 0, 0, w, 100, FEED_BUTTON[i].first, FEED_BUTTON[i].second-40, 0);

                        std::string str = std::to_string(pl->getMonsters()[acessFac.getMonsterIndex(i)].getExp());
                        str = str + "/1000";
                        al_draw_text(FontCenter::get_instance()->caviar_dreams[24], al_map_rgb(255,255,255),FEED_BUTTON[i].first+50, FEED_BUTTON[i].second-100, ALLEGRO_ALIGN_CENTRE, str.c_str());
                    }
                }
            }

            // Draw notification overlay on top
            bg_path = "./assets/image/scene/noti.png";
            al_draw_bitmap(IC->get(bg_path), 200, 20, 0);
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