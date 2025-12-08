#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "Scene.h"
#include "../facilities/Facility.h"
#include <vector>

class Menu: public Scene{
    public:
        void init();
        void update();
        void draw();
        void end();

        static Menu* get(){
            static Menu MS;
            return &MS;
        }


};

#endif