#ifndef LEVELBUTTON_H_INCLUDED
#define LEVELBUTTON_H_INCLUDED

#include "../shapes/Point.h"

class LevelButton
{
public:
  Point position;
  int level;
  bool hover;
  bool passed;
  LevelButton() : LevelButton(0, 0, 0) {}
  LevelButton(float x, float y, int level) : position{Point(x, y)}, level{level}, hover{false}, passed{false} {}
};

#endif