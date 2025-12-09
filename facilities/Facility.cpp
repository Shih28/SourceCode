#include "Facility.h"
#include "../Utils.h"
#include "../algif5/algif.h"
#include "../data/ImageCenter.h"
#include "../data/DataCenter.h"
#include "../shapes/Shape.h"
#include "../shapes/Point.h"
#include "../shapes/Rectangle.h"
#include "../Player.h"
#include "allegro5/allegro_primitives.h"

int Facility::i = 0;

void Facility::draw(){
    std::string img_path = "none";
    switch (type){
    case FARM:
        if(level==1){
            img_path = "./assets/image/facilities/farm1.png";
        }else if(level==2){
            img_path = "./assets/image/facilities/farm2.png";
        }else{
            img_path = "./assets/image/facilities/farm3.png";
        }
        break;
    case WATER_HABITAT:
        img_path = "./assets/image/facilities/water_hab.png";
        break;
    case FIRE_HABITAT:
        img_path = "./assets/image/facilities/fire_hab.png";
        break;
    case WIND_HABITAT:
        img_path = "./assets/image/facilities/wind_hab.png";
        break;
    case LIGHTNING_HABITAT:
        img_path = "./assets/image/facilities/lightning_hab.png";
        break;
    default:
        img_path = "./assets/image/facilities/empty_land.png";
        break;
    }

    if(img_path=="none") return;

    ALLEGRO_BITMAP* img = ImageCenter::get_instance()->get(img_path);
    // debug_log("<Facility> start drawing\n");
    al_draw_bitmap(img, x, y, 0);   
    al_draw_rectangle(x, y, x+width, y+length, al_map_rgb(255, 0, 0), 2);

}

void Facility::update(){
    Rectangle self = Rectangle(x, y, x+width, y+length);
    auto DC = DataCenter::get_instance();

    if(self.overlap(DC->mouse) && DC->mouse_state[1]){
        debug_log("<Facility> land triggered!\n");
        
        //change to FARM to select the type of buildings to build
        Player::getPlayer()->setAcessID(id);
        Player::getPlayer()->setrequest(Game::STATE::FARM);
    }
}