#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "types.hpp"

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

class Player {
public:
  Player() : _mode(PlayerMode::kInteractive), _hasMoved(false) {}

  std::string _name;

  int _intelligence;
  int _strength;
  int _dexterity;
  int _vitality;
  int _armor;

  int _health;
  int _mana;
  Pos _pos;

  int _level;
  int _xp;

  bool _hasMoved;

  sf::Sprite _sprite;
  PlayerMode _mode;
  PlayerClass _class;
};

#endif