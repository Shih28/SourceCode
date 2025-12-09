#include "Player.h"
#include "Utils.h"
#include <utility>
#include <vector>
#include "single_include/nlohmann/json.hpp"
#include "facilities/Facility.h"
#include <fstream>

const std::string facilitiesPath = "./database/facilitiesData.json";
using json = nlohmann::json;

const std::pair<int, int> LAND_POS[8] = {
    {200, 50},
    {350, 50},
    {500, 50},
    {650, 50},
    {200, 350},
    {350, 350},
    {500, 350},
    {650, 350}
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
                f->setPos(LAND_POS[i].first, LAND_POS[i].second);
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


    getPlayer()->setrequest(static_cast<int>(Game::STATE::MENU));
}

void Player::update(){
    //TODO
}

void Player::write(){
    if(!saveFacilities()){
        debug_log("ERROR: fail to save Facilities data!\n");
    }
}

bool Player::loadMonsters(){

}

bool Player::saveMonsters(){

}