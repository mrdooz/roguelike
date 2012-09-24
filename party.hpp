#ifndef _PARTY_HPP_
#define _PARTY_HPP_

class Player;

class Party {
  friend class Renderer;
public:

  Party() : _activePlayer(0) {}

  Player *getActivePlayer() { return _players[_activePlayer]; }

  int _activePlayer;
  std::vector<Player *> _players;
};


#endif