#pragma once

#include "player.hpp"

namespace rogue
{
  class PlayerFactory
  {
  public:
    Player *CreatePlayer(PlayerClass pc, const string& name);
  };

}
