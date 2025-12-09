#ifndef FARM_H_INCLUDED
#define FARM_H_INCLUDED

#include "Scene.h"
#include "../facilities/Facility.h"
#include <vector>

class Farm: public Scene{
    public:

        enum STATE_F{
            LAND_SETTING,
            HABITAT_MAIN,
            HABITAT_FEED,
            FARM_MAIN,
            FARM_PLANT
        };


        void init();
        void update();
        void draw();
        void end();

        static Farm* get(){
            static Farm FS;
            return &FS;
        }

    private:
    STATE_F state;

        
};

#endif