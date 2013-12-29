#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "types.hpp"
#include "roguelike.hpp"
#include "precompiled.hpp"

namespace rogue
{
  enum class PlayerClass : u8 {
    kWizard,
    kRogue,
    kWarrior,
    kCleric,
  };

  enum class PlayerMode : u8 {
    kExplore,
    kGuard,
    kFollow,
    kInteractive,
  };

  enum class PlayerAction {
    kUnknown,
    kMove,
    kMeleeAttack,
    kRangedAttack,
    kSpellAttack,
    kHeal,
  };

  string playerClassToString(PlayerClass pc);
  string playerModeToString(PlayerMode pm);


  class Player {
  public:
    Player() : _mode(PlayerMode::kInteractive), _action(PlayerAction::kUnknown), _hasMoved(false) {}

    string _name;

    int _intelligence;
    int _strength;
    int _dexterity;
    int _vitality;
    int _armor;

    int _curHealth;
    int _maxHeath;
    int _curMana;
    int _maxMana;
    Pos _pos;

    int _level;
    int _xp;

    sf::Sprite _sprite;
    PlayerMode _mode;
    PlayerClass _class;
    PlayerAction _action;
    bool _hasMoved;

  };
}

#endif