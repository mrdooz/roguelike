#pragma once

#include "player.hpp"

namespace rogue
{
  class PlayerFactory
  {
  public:
    static bool create();
    static void close();
    static PlayerFactory &instance();

    Player *CreatePlayer(PlayerClass pc);
  private:
    static PlayerFactory *_instance;  
  };

}
