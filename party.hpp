#ifndef _PARTY_HPP_
#define _PARTY_HPP_

#include "roguelike.hpp"

namespace rogue
{
  class Player;

  class Party {
    friend class Renderer;
  public:
    size_t GetNumPlayers() const { return _players.size(); }
    vector<Player *> _players;
  };
}


#endif