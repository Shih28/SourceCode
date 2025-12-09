#ifndef MONSTER_H_INCLUDED
#define MONSTER_H_INCLUDED

#include "single_include/nlohmann/json.hpp"
#include "algif5/algif.h"

using json = nlohmann::json;

class Monster{
    public:
        static int i;
        void init();
        void draw();
        void update();
        
        friend void from_json(const json &j, Monster &m){
            j.at("id").get_to(m.id);
            j.at("level").get_to(m.level);
            j.at("exp").get_to(m.exp);
            j.at("species").get_to(m.species);
            j.at("type").get_to(m.type);
        }
        friend void to_json(json &j, const Monster &m){
            j = json{{"id", m.id}, 
                {"level", m.level}, 
                {"exp", m.exp}, 
                {"species", m.species}, 
                {"type", m.type}};
        }

        enum SPECIES_M{
            UNDETERMINED,
            WATER,
            FIRE,
            WIND,
            LIGHTNING
        };

        enum TYPE_M{
            UNDEFINED
            //to be added
        };

        Monster():id{i++}, level{1}, exp{0}, species{UNDETERMINED}, type{UNDEFINED}{}
        void setType(TYPE_M t){ type=t;}
        void setSpecies(SPECIES_M s){ species=s;}
        TYPE_M getType(){ return type;}
        SPECIES_M getSpecies(){ return species;}

        ALLEGRO_BITMAP* getImg(){ return img;}
        ALLEGRO_BITMAP* getImgInStore(){ return img_in_store;}

    private:
        int id, level, exp;
        ALLEGRO_BITMAP* img; //can be extend to an array later for animation
        ALLEGRO_BITMAP* img_in_store;
        SPECIES_M species;
        TYPE_M type;
};


#endif