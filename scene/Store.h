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
            PUR_NOTI
        };

        enum NOTI_S{
            SUCCESS,
            FAIL
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
    STATE_S state, pre_state;
    NOTI_S noti;
    int noti_cnt;
    int monsters_in_display_idx[4];
        
};

#endif