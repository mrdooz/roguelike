#pragma once

namespace rogue
{
  class Player;

  class Party
  {
  public:
    size_t GetNumPlayers() const;
    vector<intrusive_ptr<Player>> _players;
  };
}
