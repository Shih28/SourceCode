#include "Player.h"
#include "Utils.h"
#include <utility>
#include <vector>
#include "single_include/nlohmann/json.hpp"
#include <unordered_map>
#include "data/ImageCenter.h"
#include "facilities/Facility.h"
#include <fstream>

const std::string facilitiesPath = "./database/facilitiesData.json";
using json = nlohmann::json;



const Monster::TYPE_M MONS_TYPE[] = {Monster::BAD_GYAUMAL};

const Monster::SPECIES_M MONS_SPEC[] = {Monster::FIRE};

const std::string MONS_IMG_WALK[] = {
    "./assets/image/monsters/fire/BadGyaumal/walk/1.png",
    "./assets/image/monsters/fire/BadGyaumal/walk/2.png",
    "./assets/image/monsters/fire/BadGyaumal/walk/3.png",
    "./assets/image/monsters/fire/BadGyaumal/walk/4.png",
    "./assets/image/monsters/fire/BadGyaumal/walk/5.png",
    "./assets/image/monsters/fire/BadGyaumal/walk/6.png",
    "./assets/image/monsters/fire/BadGyaumal/walk/7.png",
    "./assets/image/monsters/fire/BadGyaumal/walk/8.png",
    "./assets/image/monsters/fire/BadGyaumal/walk/9.png"
};

const std::string MONS_IMG_DEF[] = {
    "./assets/image/monsters/fire/BadGyaumal/default/1.png",
    "./assets/image/monsters/fire/BadGyaumal/default/2.png",
    "./assets/image/monsters/fire/BadGyaumal/default/3.png",
    "./assets/image/monsters/fire/BadGyaumal/default/4.png",
    "./assets/image/monsters/fire/BadGyaumal/default/5.png",
    "./assets/image/monsters/fire/BadGyaumal/default/6.png",
    "./assets/image/monsters/fire/BadGyaumal/default/7.png",
    "./assets/image/monsters/fire/BadGyaumal/default/8.png",
    "./assets/image/monsters/fire/BadGyaumal/default/9.png"
};

const std::string MONS_IMG_HAPPY[] = {
    "./assets/image/monsters/fire/BadGyaumal/happy/1.png",
    "./assets/image/monsters/fire/BadGyaumal/happy/2.png",
    "./assets/image/monsters/fire/BadGyaumal/happy/3.png",
    "./assets/image/monsters/fire/BadGyaumal/happy/4.png",
    "./assets/image/monsters/fire/BadGyaumal/happy/5.png",
    "./assets/image/monsters/fire/BadGyaumal/happy/6.png",
    "./assets/image/monsters/fire/BadGyaumal/happy/7.png",
    "./assets/image/monsters/fire/BadGyaumal/happy/8.png",
    "./assets/image/monsters/fire/BadGyaumal/happy/9.png"
};

const std::string MONS_STORE_PFP[] ={
    "./assets/image/monsters/fire/BadGyaumal/img_in_store.png"
};

const std::string MONS_PFP[] ={
    "./assets/image/monsters/fire/BadGyaumal/img_in_pfp.png"
};

const std::string FOOD_IMG[] = {
    "./assets/image/foods/sea_grapes.png",
    "./assets/image/foods/red_tigernut.png",
    "./assets/image/foods/fresh_junglenuts.png",
    "./assets/image/foods/rainbow_peas.png"
};

//berries, exp, price, time
const std::tuple<int, int, int, int> FOOD_DET[] = {
    {150, 1000, 800, 5},
    {400, 1500, 1000, 10},
    {1000, 3000, 2500, 15},
    {3500, 5000, 5000, 25}
};


//TODO
const std::string MONSTER_IMG[30];

const std::pair<int, int> LAND_POS[8] = {
    {50, 120},
    {380, 120},
    {710, 120},
    {1040, 120},
    {50, 400},
    {380, 400},
    {710, 400},
    {1040, 400}
};

bool Player::saveFacilities(){
    try{
        json root = json::array();
        for(const auto &f: land_settings) root.push_back(f);

        std::ofstream ofs(facilitiesPath);
        if(!ofs){
            debug_log("ERROR: failed to open .json in saveFacilities()!\n");
            return false;
        }
        ofs.clear();
        ofs << root.dump(2);
        debug_log("SUCESS: facilities' data saved!\n");
        return true;
    }catch(const std::exception& e){
        debug_log("ERROR: failed to save facilities' data!\n");
        debug_log(e.what());
        return false;
    }
}

bool Player::loadFacilties(){
    try{
        std::ifstream ifs(facilitiesPath);
        if(!ifs.is_open()){
            debug_log("WARNING: no facilitiesData.json start initialization\n");
            land_settings.clear();
            for(int i=0; i<Player::MAX_LAND; i++){
                Facility *f = new Facility();
                f->setPosMenu(LAND_POS[i].first, LAND_POS[i].second);
                land_settings.push_back(*f);
            }
            return true;
        }

        land_settings.clear();
        json root;
        ifs >> root;
        for(const auto &j: root){
            land_settings.push_back(j.get<Facility>());
        }
        debug_log("SUCESS: facilities' data loaded!\n");
        return true;

    }catch(const std::exception &e){
        debug_log("ERROR: fail to load facilities' data!\n");
        debug_log(e.what());
        return false;
    }
}

void Player::load(){
    if(!loadFacilties()){
        debug_log("ERROR: fail to load Facilities data!\n");
    }

    //coins and berries
    coin = 5000;
    berries = 1000;

    loadAllMonsters();
    loadAllFoods();
    getPlayer()->setrequest(Game::STATE::MENU);
}

void Player::update(){
    //TODO
}

void Player::write(){
    if(!saveFacilities()){
        debug_log("ERROR: fail to save Facilities data!\n");
    }
}

bool Player::loadUserMonsters(){

}

bool Player::saveUserMonsters(){

}

bool Player::loadAllMonsters(){
    auto IC = ImageCenter::get_instance();
    all_monsters.clear();

    // register images for each monster TYPE once
    // For each known TYPE_M, call Monster::registerTypeImages.
    // Here we only have BAD_GYAUMAL in constants -> register it
    {
        std::vector<std::string> walk_paths(std::begin(MONS_IMG_WALK), std::end(MONS_IMG_WALK));
        std::vector<std::string> def_paths(std::begin(MONS_IMG_DEF), std::end(MONS_IMG_DEF));
        std::vector<std::string> happy_paths(std::begin(MONS_IMG_HAPPY), std::end(MONS_IMG_HAPPY));
        Monster::registerTypeImages(Monster::TYPE_M::BAD_GYAUMAL, IC, walk_paths, def_paths, happy_paths, MONS_STORE_PFP[0], MONS_PFP[0]);
    }

    for(int i=0; i<MAX_TYPE_OF_MONSTERS; i++){
        Monster m;
        m.setType(MONS_TYPE[i]);
        m.setSpecies(MONS_SPEC[i]);
        // instance images are not stored anymore — Monster draw/getters use shared maps
        all_monsters.push_back(std::move(m));
    }
    return true;
}

bool Player::loadAllFoods(){
    auto IC = ImageCenter::get_instance();
    all_food.clear();
    int i=0;
    for(auto &s: FOOD_IMG){
        if(i>=Food::MAX_TYPE) break;
        Food f(static_cast<Food::TYPE_F>(i), std::get<0>(FOOD_DET[i]),
            std::get<1>(FOOD_DET[i]), std::get<2>(FOOD_DET[i]), std::get<3>(FOOD_DET[i]));
        
        all_food.insert({static_cast<Food::TYPE_F>(i),std::move(f)});
        Food::registerTypeImages(static_cast<Food::TYPE_F>(i), FOOD_IMG[i]);
        i++;
    }
}