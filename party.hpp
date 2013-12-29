#pragma once

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
