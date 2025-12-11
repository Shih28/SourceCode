#ifndef FARM_H_INCLUDED
#define FARM_H_INCLUDED

#include "Scene.h"
#include "../Food.h"
#include <vector>

class Farm: public Scene{
    public:
        static const int MAX_NUM = 4;
        static int page;
        enum STATE_F{
            LAND_SETTING,
            HABITAT_MAIN,
            HABITAT_MONSTERS,
            FARM_MAIN,
            LEVEL_UP,
            PUR_NOTI
        };

        enum NOTI_F{
            LEVEL_UP_SUC,
            LEVEL_UP_FAIL,
            PUR_SUC,
            PUR_FAIL
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
    STATE_F state, pre_state;
    NOTI_F noti;
    int pur_noti_cnt;
    int monster_in_display_idx[MAX_NUM];
    int food_in_display_idx[Food::MAX_TYPE];
    void updateMonstersInDisplay();
    void updateFoodInDisplay();
        
};

#endif