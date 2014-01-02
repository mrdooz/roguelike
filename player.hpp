#pragma once
#include "animated_sprite.hpp"
#include "entity.hpp"

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

  class Player : public Entity
  {
  public:
    Player();

    int _intelligence;
    int _strength;
    int _dexterity;
    int _vitality;
    int _xp;
    int _chargeRange;

    Heading _heading;
    RotatedSprite _sprite;
    PlayerMode _mode;
    PlayerClass _class;
    bool _hasMoved;

  };
}

