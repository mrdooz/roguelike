#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "game.hpp"

enum class PlayerClass : uint8 {
  kWizard,
  kRogue,
  kWarrior,
  kCleric,
};

enum class PlayerMode : uint8 {
  kExplore,
  kGuard,
  kFollow,
  kInteractive,
};

std::string playerClassToString(PlayerClass pc);

struct Player {
  Player() : _mode(PlayerMode::kInteractive), _hasMoved(false), _activePlayer(false) {}

  std::string _name;

  int _intelligence;
  int _strength;
  int _dexterity;
  int _vitality;
  int _armor;

  int _health;
  int _mana;
  Pos _pos;

  bool _activePlayer;
  bool _hasMoved;

  sf::Sprite _sprite;
  PlayerMode _mode;
  PlayerClass _class;
};

struct Party {
  Party() : _curPlayer(0) {}
  int _curPlayer;
  std::vector<Player *> _players;
};


#endif