#include "Farm.h"
#include "../Game.h"
#include "../Utils.h"
#include "../facilities/Facility.h"
#include "../Player.h"
#include "../algif5/algif.h"
#include "../data/ImageCenter.h"
#include "../data/DataCenter.h"
#include "../shapes/Rectangle.h"
#include "allegro5/allegro_primitives.h"

void Farm::init(){
    Player* pl = Player::getPlayer();
    Facility acessFac = pl->getFacilities()[pl->getAcessID()];

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

    switch (state){
        case LAND_SETTING:{
            //farm
           if(Rectangle(50, 350, 200, 500).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                acessFac.setType(Facility::TYPE_F::FARM);
                state = FARM_MAIN; 
           }//fire
           else if(Rectangle(250, 350, 400, 500).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                acessFac.setType(Facility::TYPE_F::FIRE_HABITAT);    
                state = HABITAT_MAIN;
           }//water
           else if(Rectangle(500, 350, 650, 500).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                acessFac.setType(Facility::TYPE_F::WATER_HABITAT);    
                state = HABITAT_MAIN;
           }//wind
           else if(Rectangle(750, 350, 900, 500).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                acessFac.setType(Facility::TYPE_F::WIND_HABITAT);
                state = HABITAT_MAIN;
           }//lightning
           else if(Rectangle(1000, 350, 1150, 500).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                acessFac.setType(Facility::TYPE_F::LIGHTNING_HABITAT);
                state = HABITAT_MAIN;
           }//exit
           else if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                pl->setrequest(Game::STATE::MENU);
           }
            break;
        }
        case HABITAT_MAIN:{
            //exit
           if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                pl->setrequest(Game::STATE::MENU);
           }//add monsters
           else if((Point(500, 400).overlap(DC->mouse, 40) || Point(800, 400).overlap(DC->mouse, 40)) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                state = HABITAT_MONSTERS;
           }
            break;
        }
        case HABITAT_MONSTERS: {
            //exit
            if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                pl->setrequest(Game::STATE::MENU);
            }//more monsters button
            else if(Rectangle(500, 450, 700, 600).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                pl->setrequest(Game::STATE::STORE);
            }
            break;
        }
        case FARM_MAIN:{
            //exit
            if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                pl->setrequest(Game::STATE::MENU);
            } //level up button
            else if(Rectangle(500, 450, 700, 600).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1] && acessFac.getLevel()<3){
                acessFac.getLevel()++;
                state = LEVEL_UP;
            }
            break;
        }
        case LEVEL_UP:{
            if(Rectangle(500, 450, 700, 600).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
                state = FARM_MAIN;
            }
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
            al_draw_bitmap(add, 500, 400, 0);
            al_draw_bitmap(add, 800, 400, 0);
            

            //hitboxes
            al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);
            al_draw_circle(500, 400, 40, al_map_rgb(255,0,0), 2);
            al_draw_circle(800, 400, 40, al_map_rgb(255,0,0), 2);   
            
            
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

            //more monsters button
            auto more = IC->get("./assets/image/littleStuff/more.png");
            al_draw_bitmap(more, 500, 450, 0);

            //hitboxes
            al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);
            al_draw_rectangle(500, 450, 700, 600, al_map_rgb(255,0,0), 2);
            break;
        }
        case FARM_MAIN:
        case LEVEL_UP:{
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

            //hitboxes
            al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);
            al_draw_rectangle(500, 450, 700, 600, al_map_rgb(255,0,0), 2);

            if(state==FARM_MAIN) break;
            
            //only LEVEL_UP
            bg_path = "./assets/image/scene/noti.png";
            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);

            //ok button
            auto ok = IC->get("./assets/image/littleStuff/ok.png");
            al_draw_bitmap(ok, 500, 450, 0);

            //hitboxes
            al_draw_rectangle(500, 450, 700, 600, al_map_rgb(255,0,0), 2);
            break;
        }
        default: break;
    }
}

void Farm::end(){

}