#ifndef FACILITY_H_INCLUDED
#define FACILITY_H_INCLUDED

#include "../single_include/nlohmann/json.hpp"
#include "../algif5/algif.h"
#include "../Food.h"
#include "../Monster.h"

using json = nlohmann::json;

class Facility{
    public:
        static int i;
        const int width = 300;
        const int length = 150;
        enum STATUS_F{
            EMPTY,
            IDLE,
            WORKING,
            DONE
        };
        
        enum TYPE_F{
            FARM,
            WATER_HABITAT,
            FIRE_HABITAT,
            WIND_HABITAT,
            LIGHTNING_HABITAT,
            UNDETERMINE
        };

        friend void to_json(json &j, const Facility &f){
            j = json{
            {"id", f.id},
            {"status", static_cast<int>(f.status)},
            {"type", static_cast<int>(f.type)},
            {"x", f.x},
            {"y", f.y},
            {"level", f.level}
            };
        }

        friend void from_json(const json &j, Facility &f){
            j.at("id").get_to(f.id);
            int s = 0, t = 0;
            j.at("status").get_to(s);
            j.at("type").get_to(t);
            f.status = static_cast<STATUS_F>(s);
            f.type = static_cast<TYPE_F>(t);
            j.at("x").get_to(f.x);
            j.at("y").get_to(f.y);
            j.at("level").get_to(f.level);
        }

        friend bool match(Monster &m, Facility &f){
            if(m.getSpecies()==Monster::FIRE){
                return f.type == Facility::TYPE_F::FIRE_HABITAT;
            }else if(m.getSpecies()==Monster::WATER){
                return f.type == Facility::TYPE_F::WATER_HABITAT;
            }else if(m.getSpecies()==Monster::WIND){
                return f.type == Facility::TYPE_F::WIND_HABITAT;
            }else if(m.getSpecies()==Monster::WATER){
                return f.type == Facility::TYPE_F::LIGHTNING_HABITAT;
            }

            return false;
        }
        Facility(): id{i++}, status{EMPTY}, type{UNDETERMINE}, x{0}, y{0}, level{1}, inVal{false}, inVal_cnt{0}{
            timer = al_create_timer(1);
            have_monsters[0] = false;
            have_monsters[1] = false;
            // indices into Player::getPlayer()->getMonsters(); -1 means empty
            monster_in_hab_idx[0] = -1;
            monster_in_hab_idx[1] = -1;
            
        }

        void setFood(Food::TYPE_F t){ food_type=t;}
        void setType(TYPE_F t){ type=t; }
        void setStatus(STATUS_F s){ status=s; }
        void setPosMenu(const int x_pos, const int y_pos){ x=x_pos; y=y_pos;}
        void setHaveMonsters(int i, bool t){ have_monsters[i]=t;}
        Rectangle getHitbox(){ return Rectangle(x, y, x+width, y+length);}
        int& getX(){ return x;}
        int& getY(){ return y;}
        bool getHaveMonsters(int i){ return have_monsters[i];}
        STATUS_F getStatus(){ return status;}
        TYPE_F getType(){return type;}
        Food::TYPE_F getFood(){ return food_type;}
        
        ALLEGRO_TIMER* getTimer(){return timer; }
        int& getLevel(){ return level;}

        // facility stores indices into player's monster vector
        void setMonsterIndex(int slot, int playerMonsterIndex){ 
            if(slot<0 || slot>=2) return;
            monster_in_hab_idx[slot] = playerMonsterIndex;
            have_monsters[slot] = (playerMonsterIndex >= 0);
        }
        int getMonsterIndex(int slot) const {
            if(slot<0 || slot>=2) return -1;
            return monster_in_hab_idx[slot];
        }
        void clearMonsterIndex(int slot){
            if(slot<0 || slot>=2) return;
            monster_in_hab_idx[slot] = -1;
            have_monsters[slot] = false;
        }

        void draw();
        void update();
        void timeUpdate();
    private:
        
        int id;
        int x,y; //left-upmost for menu
        int level;
        bool inVal;
        int inVal_cnt;
        Food::TYPE_F food_type;
        STATUS_F status;
        TYPE_F type;
        ALLEGRO_TIMER* timer;
        bool have_monsters[2];
        int monster_in_hab_idx[2]; // -1 = empty; value = index in Player::getMonsters()
};

#endif
