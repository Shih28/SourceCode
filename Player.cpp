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

// Monster configuration structure for easier management
struct MonsterConfig {
    Monster::TYPE_M type;
    Monster::SPECIES_M species;
    int price;
    std::string basePath;  // Base path to monster folder
    int numWalkFrames;
    int numDefFrames;
    int numHappyFrames;
};

// Configure all monsters here - easy to add new ones!
const MonsterConfig MONSTER_CONFIGS[] = {
    {
        Monster::BAD_GYAUMAL_BABY,
        Monster::FIRE,
        1200,
        "./assets/image/monsters/fire/BadGyaumal/1",
        9, 9, 9
    },
    {
        Monster::BAD_GYAUMAL_ADAULT,
        Monster::FIRE,
        2500,
        "./assets/image/monsters/fire/BadGyaumal/2",
        9, 9, 9
    }
    // Add more monsters here easily!
};

const int MAX_TYPE_OF_MONSTERS = sizeof(MONSTER_CONFIGS) / sizeof(MonsterConfig);


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

    // Loop through each monster configuration
    for(int i = 0; i < MAX_TYPE_OF_MONSTERS; i++) {
        const MonsterConfig& config = MONSTER_CONFIGS[i];
        
        debug_log("Loading monster type %d from: %s\n", config.type, config.basePath.c_str());
        
        // Build image paths dynamically
        std::vector<std::string> walk_paths;
        std::vector<std::string> def_paths;
        std::vector<std::string> happy_paths;
        
        for(int frame = 1; frame <= config.numWalkFrames; frame++) {
            walk_paths.push_back(config.basePath + "/walk/" + std::to_string(frame) + ".png");
        }
        
        for(int frame = 1; frame <= config.numDefFrames; frame++) {
            def_paths.push_back(config.basePath + "/default/" + std::to_string(frame) + ".png");
        }
        
        for(int frame = 1; frame <= config.numHappyFrames; frame++) {
            happy_paths.push_back(config.basePath + "/happy/" + std::to_string(frame) + ".png");
        }
        
        std::string store_img = config.basePath + "/img_in_store.png";
        std::string pfp_img = config.basePath + "/img_in_pfp.png";
        
        debug_log("  Store image: %s\n", store_img.c_str());
        debug_log("  Profile image: %s\n", pfp_img.c_str());
        
        // Register all images for this monster type
        Monster::registerTypeImages(config.type, IC, walk_paths, def_paths, happy_paths, store_img, pfp_img);
        
        // Create monster instance
        Monster m;
        m.setType(config.type);
        m.setSpecies(config.species);
        m.setPrice(config.price);
        all_monsters.push_back(std::move(m));
    }
    
    debug_log("Successfully loaded %d monster types\n", MAX_TYPE_OF_MONSTERS);
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