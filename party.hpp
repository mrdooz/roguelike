#ifndef _PARTY_HPP_
#define _PARTY_HPP_

#include "roguelike.hpp"

namespace rogue
{
  class Player;

  class Party
  {
  public:
    size_t GetNumPlayers() const;
    vector<Player *> _players;
  };
}

#endif