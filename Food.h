#ifndef FOOD_H_INCLUDED
#define FOOD_H_INCLUDED

#include "single_include/nlohmann/json.hpp"
#include "algif5/algif.h"
#include <vector>
#include "shapes/Rectangle.h"
#include <unordered_map>
#include <string>
#include <utility>


class Food{
    public:
        static const int MAX_TYPE = 4;
        static int i;
        static const int  width = 100, length = 100;
        void init();
        void draw();
        void update();
        
        
        enum TYPE_F{
            SEA_GRAPES,
            RED_TIGERNUT,
            JUNGLE_PEANUTS,
            RAINBOW_PEAS
        };

        Food(){}

        Food(TYPE_F t, int rb, int re, int p, int time)
            : id(Food::i++),type(t)
        {
            reward_berries = rb;
            reward_exp = re;
            price = p;
            time_need = time;
        }
        
        
        TYPE_F getType(){ return type;}
        

        int getID(){ return id;}
        int getRewBer(){ return reward_berries;}
        int getReExp(){ return reward_exp;}
        int getPrice(){ return price;}
        int getTimeNeed(){ return time_need;}

        void setPos(int xp, int yp){ x=xp; y=yp;}

        // --- Image sharing API ---
        // Register images for a Food type (call once per type at startup)
        static void registerTypeImages(TYPE_F type,const std::string img_path);

        

    private:
        int id;
        int x, y;
        
        int reward_berries, reward_exp;
        int time_need; //in seconds
        int price;
        TYPE_F type;
        
        //shared map
        static std::unordered_map<TYPE_F, ALLEGRO_BITMAP*> img_path;
};


#endif