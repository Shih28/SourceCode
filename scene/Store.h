#ifndef STORE_H_INCLUDED
#define STORE_H_INCLUDED

#include "Scene.h"
#include "../Monster.h"
#include <vector>

class Store: public Scene{
    public:
        static int page;
        
        enum STATE_S{
            ALL,
            FIRE,
            WATER,
            WIND,
            LIGHTNING,
            BUY_ATTEMPT,
            SUCESS_PUR,
            FAIL_PUR
        };


        void init();
        void update();
        void draw();
        void end();

        static Store* get(){
            static Store SS;
            return &SS;
        }

    private:
    void updateMonstersInDisplay();
    STATE_S state;
    int monsters_in_display_idx[4];
        
};

#endif