#ifndef FACILITY_H_INCLUDED
#define FACILITY_H_INCLUDED

#include "../single_include/nlohmann/json.hpp"
#include "../algif5/algif.h"

using json = nlohmann::json;

class Facility{
    public:
        static int i;
        const int width = 100;
        const int length = 100;
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
                {"reward", f.reward}
            };
        }

        friend void from_json(const json &j, Facility &f){
            j.at("id").get_to(f.id);
            int s = 0, t = 0;
            j.at("status").get_to(s);
            j.at("type").get_to(t);
            f.status = static_cast<STATUS_F>(s);
            f.type = static_cast<TYPE_F>(t);
            j.at("reward").get_to(f.reward);
        }

        Facility(): id{i++}, status{EMPTY}, type{UNDETERMINE}, reward{0}, x{0}, y{0}, level{1}{
            timer = al_create_timer(1);
        }

        void setType(TYPE_F t){ type=t; }
        void setStatus(STATUS_F s){ status=s; }
        void setReward(int r){ reward=r; }
        void setPos(const int x_pos, const int y_pos){ x=x_pos; y=y_pos;}
        STATUS_F getStatus(){ return status;}
        TYPE_F getType(){return type;}
        int getReward(){return reward; }
        ALLEGRO_TIMER* getTimer(){return timer; }

        void draw();
        void update();
    private:
        
        int id;
        int x,y; //left-upmost
        int level;
        STATUS_F status;
        TYPE_F type;
        int reward;
        ALLEGRO_TIMER* timer;
        
        //TODO: monster_in_it
        
    
};




#endif