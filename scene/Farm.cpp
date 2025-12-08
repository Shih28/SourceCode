#include "Farm.h"
#include "../Game.h"
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
           if(Rectangle(100, 400, 200, 450).overlap(DC->mouse) && DC->mouse_state[1]){
                state = FARM_MAIN;
           }else if(Rectangle(800, 400, 900, 450).overlap(DC->mouse) && DC->mouse_state[1]){
                state = HABITAT_MAIN;
           }else if(Rectangle(450, 400, 500, 450).overlap(DC->mouse) && DC->mouse_state[1]){
                pl->setrequest(static_cast<int>(Game::STATE::MENU));
           }
            break;
        }
        case HABITAT_MAIN:{
            if(Rectangle(100, 400, 200, 450).overlap(DC->mouse) && DC->mouse_state[1]){
                state = HABITAT_FEED;
            }else if(Rectangle(450, 400, 500, 450).overlap(DC->mouse) && DC->mouse_state[1]){
                pl->setrequest(static_cast<int>(Game::STATE::MENU));
           }
            break;
        }
        case HABITAT_FEED:{
            if(Rectangle(800, 400, 900, 450).overlap(DC->mouse) && DC->mouse_state[1]){
                state = HABITAT_MAIN;
            }
            break;
        }
        case FARM_MAIN:{
            if(Rectangle(100, 400, 200, 450).overlap(DC->mouse) && DC->mouse_state[1]){
                state = FARM_PLANT;
            }else if(Rectangle(450, 400, 500, 450).overlap(DC->mouse) && DC->mouse_state[1]){
                pl->setrequest(static_cast<int>(Game::STATE::MENU));
            }
            break;
        }
        case FARM_PLANT:{
            if(Rectangle(800, 400, 900, 450).overlap(DC->mouse) && DC->mouse_state[1]){
                state = FARM_MAIN;
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
            bg_path = "./assets/image/scene/land_setting.jpg";

            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);
            //farm
            al_draw_filled_rectangle(100, 400, 200, 450, al_map_rgb(0,0,255));
            //habitat
            al_draw_filled_rectangle(800, 400, 900, 450, al_map_rgb(255,0,0));

            //exit
            al_draw_filled_rectangle(450, 400, 500, 450, al_map_rgb(0, 255, 0));
            break;
        }
        case HABITAT_MAIN:{
            bg_path = "./assets/image/scene/hab_main.jpg";
            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);

            al_draw_filled_rectangle(100, 400, 200, 450, al_map_rgb(0,0,255));
            //exit
            al_draw_filled_rectangle(450, 400, 500, 450, al_map_rgb(0, 255, 0));
            break;
        }
        case HABITAT_FEED:{
            bg_path = "./assets/image/scene/hab_feed.jpg";
            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);

            al_draw_filled_rectangle(800, 400, 900, 450, al_map_rgb(255,0,0));
            
            break;
        }
        case FARM_MAIN:{
            bg_path = "./assets/image/scene/farm_main.jpg";
            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);

            al_draw_filled_rectangle(100, 400, 200, 450, al_map_rgb(0,0,255));
            //exit
            al_draw_filled_rectangle(450, 400, 500, 450, al_map_rgb(0, 255, 0));
            break;
        }
        case FARM_PLANT:{
            bg_path = "./assets/image/scene/farm_plant.jpg";
            al_draw_bitmap(IC->get(bg_path), 0, 0, 0);

            al_draw_filled_rectangle(800, 400, 900, 450, al_map_rgb(255,0,0));

            break;
        }

        default: break;
    }
}

void Farm::end(){

}