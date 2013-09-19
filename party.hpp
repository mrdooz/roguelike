#ifndef _PARTY_HPP_
#define _PARTY_HPP_

namespace rogue
{
  class Player;

  class Party {
    friend class Renderer;
  public:

    Party() : _activePlayer(0) {}

    Player *getActivePlayer() { return _players[_activePlayer]; }

    size_t _activePlayer;
    vector<Player *> _players;
  };
}


#endif