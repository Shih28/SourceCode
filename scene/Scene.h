#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

class Scene{
    public:
    virtual void init()=0;
    virtual void update()=0;
    virtual void draw()=0;
    virtual void end()=0;
    

};


#endif