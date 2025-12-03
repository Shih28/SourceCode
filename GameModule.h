#ifndef GAMEMODULE_H_INCLUDED
#define GAMEMODULE_H_INCLUDED

#include <optional>
#include "Game.h"

class GameModule
{
public:
  virtual void init() {};
  virtual void update() {};
  virtual void draw() {};
  virtual void end() {};
  virtual std::optional<Game::STATE> get_game_state_update_request() { return std::nullopt; };
};

#endif