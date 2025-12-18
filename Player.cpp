#include "Player.h"
#include "Utils.h"
#include <utility>
#include <vector>
#include "single_include/nlohmann/json.hpp"
#include <unordered_map>
#include "data/ImageCenter.h"
#include "facilities/Facility.h"
#include <fstream>

const std::string DATA_PATHS[] = 
    {"./database/PlayerData.json",
     "./database/FacilitiesData.json",
     "./database/MonsterData.json"
    };


const std::pair<int, int> MONS_POS_FEED_MENU[] = {
    {150, 200},
    {753,  200}
};

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
    },
    {
        Monster::FREETTLE_BABY,
        Monster::FIRE,
        1000,
        "./assets/image/monsters/fire/Freettle/1",
        9, 9, 9
    },
    {
        Monster::FREETTLE_ADAULT,
        Monster::FIRE,
        1000,
        "./assets/image/monsters/fire/Freettle/2",
        9, 9, 9
    },
    {
        Monster::DAKUABENJA_BABY,
        Monster::WATER,
        1500,
        "./assets/image/monsters/water/Dakuabenja/1",
        9,9,9
    },
    {
        Monster::DAKUABENJA_ADAULT,
        Monster::WATER,
        1500,
        "./assets/image/monsters/water/Dakuabenja/2",
        9,9,9
    },
    {
        Monster::LORD_OF_ATL_BABY,
        Monster::WATER,
        1800,
        "./assets/image/monsters/water/LordOfAtlantis/1",
        9,9,9
    },
    {
        Monster::LORD_OF_ATL_ADAULT,
        Monster::WATER,
        1800,
        "./assets/image/monsters/water/LordOfAtlantis/2",
        9,9,9
    },
    {
        Monster::PANDALF_BABY,
        Monster::WIND,
        2500,
        "./assets/image/monsters/wind/Pandalf/1",
        9, 9, 9
    },
    {
        Monster::PANDALF_ADAULT,
        Monster::WIND,
        2500,
        "./assets/image/monsters/wind/Pandalf/2",
        9, 9, 9
    },
    {
        Monster::VIRELIA_BABY,
        Monster::WIND,
        2300,
        "./assets/image/monsters/wind/Virelia/1",
        9, 9, 9
    },
    {
        Monster::VIRELIA_ADAULT,
        Monster::WIND,
        2300,
        "./assets/image/monsters/wind/Virelia/2",
        9, 9, 9
    },
    {
        Monster::FENNEVYR_BABY,
        Monster::LIGHTNING,
        2300, 
        "./assets/image/monsters/lightning/Fennevyr/1",
        9, 9, 9
    },
    {
        Monster::FENNEVYR_ADAULT,
        Monster::LIGHTNING,
        2300, 
        "./assets/image/monsters/lightning/Fennevyr/2",
        9, 9, 9
    },
    {
        Monster::THORDER_BABY,
        Monster::LIGHTNING,
        2500,
        "./assets/image/monsters/lightning/Thorder/1",
        9, 9, 9
    },
    {
        Monster::THORDER_ADAULT,
        Monster::LIGHTNING,
        2500,
        "./assets/image/monsters/lightning/Thorder/2",
        9, 9, 9
    }
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
    {100, 165},
    {420, 145},
    {750, 135},
    {1060, 165},

    {50, 400},
    {380, 470},
    {710, 475},
    {1040, 400}
};



bool Player::saveFacilities(){
    try{
        json root = json::array();
        for(const auto &f: land_settings) root.push_back(f);

        std::ofstream ofs(DATA_PATHS[1]);
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
        std::ifstream ifs(DATA_PATHS[1]);
        if(!ifs.is_open()){
            debug_log("WARNING: no facilitiesData.json start initialization\n");
            land_settings.clear();
            for(int i=0; i<Player::MAX_LAND; i++){
                Facility *f = new Facility();
                f->setPosMenu(LAND_POS[i].first, LAND_POS[i].second);
                land_settings.push_back(*f);
            }
            return false;
        }

        land_settings.clear();
        json root;
        ifs >> root;
        int i=0;
        for(const auto &j: root){
            land_settings.push_back(j.get<Facility>());
            land_settings[i].setPosMenu(LAND_POS[i].first, LAND_POS[i].second);
            auto timer = land_settings[i].getTimer();
            
            if(land_settings[i].getStatus()==Facility::STATUS_F::WORKING){
                al_set_timer_count(timer, land_settings[i].getTimeCnt());
                al_start_timer(timer);
            }else{
                al_set_timer_count(timer, 0);
                al_stop_timer(timer);
            }

            //initialize monster's data if in HABITAT
            if(land_settings[i].getHaveMonsters(0)){
                int idx = land_settings[i].getMonsterIndex(0);
                monster_owned[idx].setPosFeed(MONS_POS_FEED_MENU[0].first, MONS_POS_FEED_MENU[0].second);
                monster_owned[idx].setPosMenu(LAND_POS[i].first, LAND_POS[i].second);
                monster_owned[idx].setPlacing(Monster::PLACE_M::HABITAT);
                monster_owned[idx].setFacilityRec(land_settings[i].getHitbox());
            }


            if(land_settings[i].getHaveMonsters(1)){
                int idx = land_settings[i].getMonsterIndex(1);
                monster_owned[idx].setPosFeed(MONS_POS_FEED_MENU[1].first, MONS_POS_FEED_MENU[1].second);
                monster_owned[idx].setPosMenu(LAND_POS[i].first, LAND_POS[i].second);
                monster_owned[idx].setPlacing(Monster::PLACE_M::HABITAT);
                monster_owned[idx].setFacilityRec(land_settings[i].getHitbox());
            }
            i++;
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

    //player's data
    if(!loadPlayerData()){
        debug_log("ERROR: fail to load user's Player's data!\n");
    }

    //user's monsters' data
    if(!loadUserMonsters()){
        debug_log("ERROR: fail to load user's Monster's data!\n");
    }

    //user's facilities' data
    if(!loadFacilties()){
        debug_log("ERROR: fail to load user's Facilities data!\n");
    }

    //data references
    loadAllMonsters();
    loadAllFoods();
    getPlayer()->setrequest(Game::STATE::MENU);
}

void Player::update(){
    //TODO
}

void Player::write(){

    //player's data
    if(!savePlayerData()){
        debug_log("ERROR: fail to save user's Player's data!\n");
    }

    //user's facilities' data
    if(!saveFacilities()){
        debug_log("ERROR: fail to save user's Facilities data!\n");
    }

    //user's monster's data
    if(!saveUserMonsters()){
        debug_log("ERROR: fail to save user's Monsters data!\n");
    }
}

bool Player::loadUserMonsters(){

    try{
        std::ifstream ifs(DATA_PATHS[2]);
        if(!ifs.is_open()){
            debug_log("WARNING: no facilitiesData.json start initialization\n");
            return false;
        }
        monster_owned.clear();
        json root;
        ifs >> root;
        for(const auto &j: root){
            monster_owned.push_back(j.get<Monster>());
        }
        debug_log("SUCESS: Monsters data loaded!\n");
        return true;
        
    }catch(const std::exception &e){
        debug_log("ERROR: fail to load Monsters data!\n");
        debug_log(e.what());
        return false;
    }
    
}

bool Player::saveUserMonsters(){
    try{
        json root = json::array();
        for(const auto &m: monster_owned) root.push_back(m);

        std::ofstream ofs(DATA_PATHS[2]);
        if(!ofs){
            debug_log("ERROR: failed to open MonsterData.json in saveUserMonsters()!\n");
            return false;
        }
        
        ofs << root.dump(2);
        debug_log("SUCCESS: Monsters data saved!\n");
        return true;
        
    }catch(const std::exception& e){
        debug_log("ERROR: failed to save Monsters data!\n");
        debug_log(e.what());
        return false;
    }
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

    return true;
}

bool Player::savePlayerData(){
    try{
        json j = *this;  // Automatically calls to_json friend function
        
        std::ofstream ofs(DATA_PATHS[0]);
        if(!ofs){
            debug_log("ERROR: failed to create PlayerData.json!\n");
            return false;
        }
        
        ofs << j.dump(2);
        debug_log("SUCCESS: Player data saved!\n");
        return true;
        
    }catch(const std::exception& e){
        debug_log("ERROR: failed to save Player's data!\n");
        debug_log(e.what());
        return false;
    }
}

bool Player::loadPlayerData(){
    try{
        std::ifstream ifs(DATA_PATHS[0]);
        if(!ifs.is_open()){
            debug_log("WARNING: no PlayerData.json, start initialization\n");
            coin = 10000;
            berries = 5000;
            exp = 0;
            return false;
        }

        json root;
        ifs >> root;
        from_json(root, *this);
        
        debug_log("SUCCESS: Player data loaded!\n");
        return true;
        
    }catch(const std::exception &e){
        debug_log("ERROR: fail to load Player's data!\n");
        debug_log(e.what());
        return false;
    }
}
/*
* @brief in-game initialization
*/
bool Player::initializeAllData(){
    
    auto pl = Player::getPlayer();
    //player data reset
    pl->getCoin() = 10000;
    pl->getBer() = 5000;
    //monster & facilities
    pl->getMonsters().clear();
    for(auto &f: pl->getFacilities()){
        f.setStatus(Facility::STATUS_F::EMPTY);
    }
    return true;

}