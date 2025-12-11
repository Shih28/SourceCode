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
#include "allegro5/allegro_font.h"
#include "../data/FontCenter.h"
#include "../Food.h"

int Facility::i = 0;
const int BAR_LENGTH = 200;
const int BAR_HEIGHT = 15;
const std::string BAR_IMG[] = {
    "./assets/image/littleStuff/blue_bar.png",
    "./assets/image/littleStuff/yellow_bar.png"
};

const std::string BERRY_IMG = "./assets/image/littleStuff/berries.png";
const std::string COIN_IMG = "./assets/image/littleStuff/coin.png";


void Facility::draw(){
    auto pl = Player::getPlayer();
    auto IC = ImageCenter::get_instance();
    std::string img_path = "none";

    //draw buildings
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

    ALLEGRO_BITMAP* img = IC->get(img_path);
    // debug_log("<Facility> start drawing\n");
    al_draw_bitmap(img, x, y, 0); 
    al_draw_rectangle(x, y+50, x+width, y+50+length, al_map_rgb(255, 0, 0), 2);

    //for farms
    if(status==WORKING && type==FARM){
        auto blue_bar = IC->get(BAR_IMG[0]);
        auto yellow_bar = IC->get(BAR_IMG[1]);
        auto &food = (*pl->getAllFoods().find(food_type)).second;
        double scale = ((double)al_get_timer_count(timer))/((double)food.getTimeNeed());
        double w = scale*BAR_LENGTH;

        // debug_log("w: %lf\n", w);
        al_draw_bitmap(blue_bar, x, y+length, 0);
        al_draw_bitmap_region(yellow_bar, 0, 0, w, 100, x, y+length, 0);

    }else if(status==DONE && type==FARM){
        auto berries = IC->get(BERRY_IMG);
        al_draw_bitmap(berries, x+width/2, y-15, 0);
        al_draw_rectangle(x+width/2, y-15, x+width, y, al_map_rgb(255, 0, 0), 2);
    }

    //for habitats
    if(status==DONE && type!=FARM){
        auto coin = IC->get(COIN_IMG);
        al_draw_bitmap(coin, x+width/2, y-15, 0);
        al_draw_rectangle(x+width/2, y-15, x+width, y, al_map_rgb(255, 0, 0), 2);
    }

    //draw invalid message
    if(inVal){
        char str[] = "The farm is occupied!";
        al_draw_text(FontCenter::get_instance()->caviar_dreams[36], al_map_rgb(255,255,255), 640, 360, ALLEGRO_ALIGN_CENTRE, str);
    }
}

void Facility::update(){
    Rectangle self = Rectangle(x, y, x+width, y+length);
    auto pl = Player::getPlayer();
    auto DC = DataCenter::get_instance();

    //when facility is clicked
    if(self.overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        
        if(type==FARM && status==WORKING){
            inVal=true;
            goto SKIP;
        }

        // automatically collects
        if(status==DONE && type==FARM){
            //berries
            auto &food = (*pl->getAllFoods().find(food_type)).second;
            
            pl->getBer() += food.getRewBer();
            pl->getExp() += food.getReExp();
            status = IDLE;
        }else if(status==DONE && type!=FARM){
            //coins
            pl->getCoin() += 1000;
            status = WORKING;
            al_start_timer(timer);
        }else{
            //change to FARM to select the type of buildings to build
            Player::getPlayer()->setAcessID(id);
            Player::getPlayer()->setrequest(Game::STATE::FARM);
        }
    }

    SKIP:

    //when bubble is clicked
    if(status==DONE && Rectangle(x+width/2, y-15, x+width, y).overlap(DC->mouse) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        if(type==FARM){
            //berries
            auto &food = (*pl->getAllFoods().find(food_type)).second;
            pl->getBer() += food.getRewBer();
            pl->getExp() += food.getReExp();
            status = IDLE;
        }else{
            //coins
            int cnt=0;
            cnt = (have_monsters[0])? cnt+1: cnt;
            cnt = (have_monsters[1])? cnt+1: cnt;
            pl->getCoin() += 1000*cnt;
            status = WORKING;
            al_start_timer(timer);
        }
    }

    if(inVal && inVal_cnt<=60){
        inVal_cnt++;
    }else{
        inVal=false;
        inVal_cnt=0;
    }

}

void Facility::timeUpdate(){
    auto pl = Player::getPlayer();

    switch (status){
        case EMPTY:{
            break;
        }
        case IDLE:{
            al_set_timer_count(timer, 0);
            al_stop_timer(timer);
            break;
        }
        case WORKING:{
            if(type==FARM){
                auto &food = (*pl->getAllFoods().find(food_type)).second;
                if(al_get_timer_count(timer) >= food.getTimeNeed()){
                    status = DONE;
                }
            }else{
                if(al_get_timer_count(timer) >= 10){
                    status = DONE;
                }
            }
            // debug_log("time_cnt: %d\n", al_get_timer_count(timer));
            break;
        }
        case DONE:{
            al_set_timer_count(timer, 0);
            al_stop_timer(timer);
            break;
        }
    }

}