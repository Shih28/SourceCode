#ifndef PROFILE_H_INCLUDED
#define PROFILE_H_INCLUDED

#include "Scene.h"
#include <vector>

class Profile: public Scene{
    public:
        void init();
        void update();
        void draw();
        void end();

        static Profile* get(){
            static Profile PS;
            return &PS;
        }

        
};

#endif