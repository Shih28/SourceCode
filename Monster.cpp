#include "Monster.h"
#include "shapes/Rectangle.h"
#include "allegro5/allegro_primitives.h"
#include "Utils.h"
#include "data/ImageCenter.h" // for ImageCenter type/usage

int Monster::i = 0;

// static map definitions
std::unordered_map<Monster::TYPE_M, std::vector<ALLEGRO_BITMAP*>> Monster::s_walk_map;
std::unordered_map<Monster::TYPE_M, std::vector<ALLEGRO_BITMAP*>> Monster::s_def_map;
std::unordered_map<Monster::TYPE_M, std::vector<ALLEGRO_BITMAP*>> Monster::s_happy_map;
std::unordered_map<Monster::TYPE_M, ALLEGRO_BITMAP*> Monster::s_store_map;
std::unordered_map<Monster::TYPE_M, ALLEGRO_BITMAP*> Monster::s_pfp_map;

void Monster::registerTypeImages(TYPE_M type,
                                 ImageCenter* IC,
                                 const std::vector<std::string>& walk_paths,
                                 const std::vector<std::string>& def_paths,
                                 const std::vector<std::string>& happy_paths,
                                 const std::string& store_path,
                                 const std::string& pfp_path)
{
    // Avoid reloading if already registered
    if(s_walk_map.find(type) != s_walk_map.end()) return;

    std::vector<ALLEGRO_BITMAP*> walk_frames;
    for(const auto &p : walk_paths){
        walk_frames.push_back(IC->get(p));
    }
    s_walk_map[type] = std::move(walk_frames);

    std::vector<ALLEGRO_BITMAP*> def_frames;
    for(const auto &p : def_paths){
        def_frames.push_back(IC->get(p));
    }
    s_def_map[type] = std::move(def_frames);

    std::vector<ALLEGRO_BITMAP*> happy_frames;
    for(const auto &p : happy_paths){
        happy_frames.push_back(IC->get(p));
    }
    s_happy_map[type] = std::move(happy_frames);

    s_store_map[type] = IC->get(store_path);
    s_pfp_map[type] = IC->get(pfp_path);
}

ALLEGRO_BITMAP* Monster::getWalkFrame(int frame) const {
    auto it = s_walk_map.find(type);
    if(it == s_walk_map.end() || it->second.empty()) return nullptr;
    int idx = frame % (int)it->second.size();
    return it->second[idx];
}

ALLEGRO_BITMAP* Monster::getDefFrame(int frame) const {
    auto it = s_def_map.find(type);
    if(it == s_def_map.end() || it->second.empty()) return nullptr;
    int idx = frame % (int)it->second.size();
    return it->second[idx];
}

ALLEGRO_BITMAP* Monster::getHappyFrame(int frame) const {
    auto it = s_happy_map.find(type);
    if(it == s_happy_map.end() || it->second.empty()) return nullptr;
    int idx = frame % (int)it->second.size();
    return it->second[idx];
}

ALLEGRO_BITMAP* Monster::getImgInStore() const {
    auto it = s_store_map.find(type);
    return (it == s_store_map.end()) ? nullptr : it->second;
}
ALLEGRO_BITMAP* Monster::getImgInPfp() const {
    auto it = s_pfp_map.find(type);
    return (it == s_pfp_map.end()) ? nullptr : it->second;
}

void Monster::init(){

}


void Monster::update(){
    if(exp>=EXP && level==1){
        exp = EXP;
        level++;
        type = static_cast<TYPE_M>(static_cast<int>(type)+1);
    }

    switch(place){
        case HABITAT:{
            status = WALK;

            if(x+dx<(int)its_habitat.leftmost()-50|| x+width+dx>(int)its_habitat.rightmost()+50){
                dx = -dx;
                direction = !direction;
            }

            if(y+length+dy>(int)its_habitat.downmost()+50|| y+dy<(int)its_habitat.upmost()-50){
                dy = -dy;
            }

            x += dx;
            y += dy;
            
            break;
        }
        case FEEDING:{
            if(feed){
                if(finishedMovement()){
                    feed=false;
                    status = DEFAULT;
                }else{
                    status = HAPPY;
                } 
            }else{
                status = DEFAULT;
            }
            break;
        }
        case NONE:{
            status = NO_DISPLAY;
            break;
        }
    }

}

void Monster::draw(){
    switch(status){
        case WALK: {
            auto dir = direction? 0: ALLEGRO_FLIP_HORIZONTAL;
            ALLEGRO_BITMAP* bmp = getWalkFrame(display_cnt/8);
            if(bmp) al_draw_bitmap(bmp, x, y, dir);
            display_cnt = display_cnt==MAX_ANIMATION*8-1? 0: display_cnt+1;
            break;
        }
        case HAPPY: {
            ALLEGRO_BITMAP* bmp = getHappyFrame(display_cnt/8);
            if(bmp) al_draw_bitmap(bmp, x_f, y_f, 0);
            display_cnt = display_cnt==MAX_ANIMATION*8-1? 0: display_cnt+1;
            break;
        }
        case DEFAULT: {
            ALLEGRO_BITMAP* bmp = getDefFrame(display_cnt/8);
            if(bmp) al_draw_bitmap(bmp, x_f, y_f, 0);
            display_cnt = display_cnt==MAX_ANIMATION*8-1? 0: display_cnt+1;
            break;
        }
        case NO_DISPLAY: {
            break;
        }
    }

    al_draw_rectangle(x, y, x+width, y+length, al_map_rgb(255,0,0),2);
}