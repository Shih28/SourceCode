#include "Food.h"
#include "data/ImageCenter.h"
#include "Utils.h"
#include <unordered_map>

int Food::i = 0;
std::unordered_map<Food::TYPE_F, ALLEGRO_BITMAP*> Food::img_path;
void Food::init(){

}

void Food::draw(){
    ALLEGRO_BITMAP* img = img_path.find(type)->second;
    if(img != nullptr){
        al_draw_bitmap(img, x, y, 0);
        // debug_log("draw_pos: %d, %d\n", x, y);
    }
}

void Food::update(){

}

void Food::registerTypeImages(TYPE_F type, const std::string imgPath){
    auto IC = ImageCenter::get_instance();
    img_path.insert({type, IC->get(imgPath)});
    
}