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
    friend void intrusive_ptr_add_ref(Player*);
    friend void intrusive_ptr_release(Player*);

  public:
    Player();

    void DebugDump(vector<string>& dump);
    virtual bool IsHuman() const { return true; }

    int _intelligence;
    int _strength;
    int _dexterity;
    int _vitality;
    int _xp;
    int _xpForNextLevel;
    int _chargeRange;
    int _gold;
    int _manaPotions;
    int _healthPotions;

    Heading _heading;
    RotatedSprite _sprite;
    PlayerMode _mode;
    PlayerClass _class;
    bool _hasMoved;
  };

  typedef intrusive_ptr<Player> PlayerPtr;

  //-----------------------------------------------------------------------------
  inline void intrusive_ptr_add_ref(Player* e)
  {
    ++e->_refCount;
  }

  //-----------------------------------------------------------------------------
  inline void intrusive_ptr_release(Player* e)
  {
    if (--e->_refCount == 0)
    {
      delete e;
    }
  }

}

