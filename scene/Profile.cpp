#include "Profile.h"
#include "../Player.h"
#include "../data/ImageCenter.h"
#include "../data/DataCenter.h"
#include "../shapes/Point.h"
#include "allegro5/allegro_primitives.h"

void Profile::init(){

}

void Profile::update(){
    auto pl = Player::getPlayer();
    auto DC = DataCenter::get_instance();
    //exit  
    if(Point(1100, 10).overlap(DC->mouse, 40) && DC->mouse_state[1] && !DC->prev_mouse_state[1]){
        pl->setrequest(Game::STATE::MENU);
    }
}

void Profile::draw(){
    Player *pl = Player::getPlayer();
    std::vector<Monster> lib = pl->getAllMonsters();
    auto IC = ImageCenter::get_instance();
    auto bg = IC->get("./assets/image/scene/pfp.png");

    //background
    al_draw_bitmap(bg, 0, 0, 0);

    //exit
    auto exit = IC->get("./assets/image/littleStuff/exit.png");
    al_draw_bitmap(exit, 1100, 10, 0);

    //hitboxes
    al_draw_circle(1100, 10, 40, al_map_rgb(255,0,0), 2);


}
void Profile::end(){
    
}