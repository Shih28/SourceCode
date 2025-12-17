#include "Food.h"
#include "data/ImageCenter.h"
#include "Utils.h"
#include <unordered_map>

int Food::i = 0;
std::unordered_map<Food::TYPE_F, ALLEGRO_BITMAP*> Food::img_path;
std::unordered_map<Food::TYPE_F, ALLEGRO_BITMAP*> Food::img_path_hover;



void Food::init(){

}

void Food::draw(bool hover){
    // Use hover image if hovering and it exists, otherwise use normal image
    ALLEGRO_BITMAP* img = nullptr;
    
    if(hover) {
        auto hover_it = img_path_hover.find(type);
        if(hover_it != img_path_hover.end()) {
            img = hover_it->second;
        }
    }
    
    // Fall back to normal image if no hover image or not hovering
    if(img == nullptr) {
        auto it = img_path.find(type);
        if(it != img_path.end()) {
            img = it->second;
        }
    }
    
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
    
    // Generate hover image path by adding "2" before ".png"
    size_t dotPos = imgPath.rfind(".png");
    if(dotPos != std::string::npos) {
        std::string hoverPath = imgPath.substr(0, dotPos) + "2.png";
        img_path_hover.insert({type, IC->get(hoverPath)});
    }
}