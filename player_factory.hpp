#ifndef _PLAYER_FACTORY_HPP_
#define _PLAYER_FACTORY_HPP_

#include "player.hpp"

namespace rogue
{
  class PlayerFactory
  {
  public:
    static bool create();
    static void close();
    static PlayerFactory &instance();

    Player *createPlayer(PlayerClass pc);
  private:
    static PlayerFactory *_instance;  
  };

}


#endif
