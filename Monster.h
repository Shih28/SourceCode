#ifndef MONSTER_H_INCLUDED
#define MONSTER_H_INCLUDED

#include "single_include/nlohmann/json.hpp"
#include "algif5/algif.h"
#include <vector>
#include "shapes/Rectangle.h"
#include <unordered_map>
#include <string>

using json = nlohmann::json;

class Monster{
    public:
        static const int MAX_ANIMATION = 9;
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

        enum PLACE_M{
            HABITAT,
            FEEDING,
            NONE
        };

        enum STATUS_M{
            WALK,
            HAPPY,
            DEFAULT,
            NO_DISPLAY
        };

        enum TYPE_M{
            UNDEFINED,
            BAD_GYAUMAL
            //to be added
        };

        Monster()
            : id(Monster::i++), level(1), exp(0),
            x(0), y(0), center_x(0), center_y(0),
            width(50), length(80), dx(1), dy(0),
            species(SPECIES_M::FIRE), type(TYPE_M::BAD_GYAUMAL),
            status(STATUS_M::NO_DISPLAY), place(PLACE_M::NONE),
            display_cnt(0), direction(true), feed(false)
        {
        }
        void setType(TYPE_M t){ type=t;}
        void setSpecies(SPECIES_M s){ species=s;}
        void setPlacing(PLACE_M p){ place=p;}
        void setWidth(int w){ width=w;}
        void setLength(int l){ length=l;}
        void setFacilityRec(Rectangle r){
            its_habitat = r;
        }
        void setPosMenu(int xp, int yp){ 
            x=xp; y=yp;
            center_x = x +width/2; center_x = y + length/2;
        }
        void setPosFeed(int xp, int yp){
            x_f=xp; y_f=yp;
        }
        
        PLACE_M getPlacing(){ return place;}
        STATUS_M getStatus(){ return status;}
        TYPE_M getType(){ return type;}
        SPECIES_M getSpecies(){ return species;}

        int getID(){ return id;}

        // --- Image sharing API ---
        // Register images for a monster type (call once per type at startup)
        static void registerTypeImages(TYPE_M type,
                                       class ImageCenter* IC,
                                       const std::vector<std::string>& walk_paths,
                                       const std::vector<std::string>& def_paths,
                                       const std::vector<std::string>& happy_paths,
                                       const std::string& store_path,
                                       const std::string& pfp_path);

        // getters (instances forward to the shared maps)
        ALLEGRO_BITMAP* getWalkFrame(int frame) const;
        ALLEGRO_BITMAP* getDefFrame(int frame) const;
        ALLEGRO_BITMAP* getHappyFrame(int frame) const;
        ALLEGRO_BITMAP* getImgInStore() const;
        ALLEGRO_BITMAP* getImgInPfp() const;

    private:
        int id, level, exp;
        int x, y; //left-up-most
        int x_f, y_f; //left-up-most for feed menu
        int center_x, center_y;
        int width, length;
        int dx, dy;

        SPECIES_M species;
        TYPE_M type;
        STATUS_M status;
        PLACE_M place;
        int display_cnt;
        bool direction;
        bool feed;
        Rectangle its_habitat;
        bool finishedMovement(){ return display_cnt>=MAX_ANIMATION*8-1;}

        // shared image maps
        static std::unordered_map<TYPE_M, std::vector<ALLEGRO_BITMAP*>> s_walk_map;
        static std::unordered_map<TYPE_M, std::vector<ALLEGRO_BITMAP*>> s_def_map;
        static std::unordered_map<TYPE_M, std::vector<ALLEGRO_BITMAP*>> s_happy_map;
        static std::unordered_map<TYPE_M, ALLEGRO_BITMAP*> s_store_map;
        static std::unordered_map<TYPE_M, ALLEGRO_BITMAP*> s_pfp_map;
};


#endif