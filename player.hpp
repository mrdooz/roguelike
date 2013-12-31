#pragma once
#include "animated_sprite.hpp"

namespace rogue
{
  enum class PlayerClass : u8
  {
    kWizard,
    kRanger,
    kWarrior,
    kCleric,
  };

  enum class PlayerMode : u8
  {
    kExplore,
    kGuard,
    kFollow,
    kInteractive,
  };

  string playerClassToString(PlayerClass pc);
  string playerModeToString(PlayerMode pm);

  class Player
  {
  public:
    Player();

    string _name;

    int _intelligence;
    int _strength;
    int _dexterity;
    int _vitality;
    int _armor;

    int _armorBonus;
    int _weaponBonus;

    int _curHealth;
    int _maxHeath;
    int _curMana;
    int _maxMana;

    int _level;
    int _xp;
    int _chargeRange;

    Heading _heading;
    Pos _pos;
    RotatedSprite _sprite;
    PlayerMode _mode;
    PlayerClass _class;
    bool _hasMoved;

  };
}

