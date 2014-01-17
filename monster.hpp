#pragma once

#include "entity.hpp"
#include "hotload_sprite.hpp"

namespace rogue
{
  class Player;

  class Monster : public Entity
  {
    friend void intrusive_ptr_add_ref(Monster*);
    friend void intrusive_ptr_release(Monster*);

  public:
    enum class Type
    {
      Goblin,
      Skeleton,
      SkeletonWarrior,
      SkeletonMage,
      FireElemental,
      WaterElemental,
      Ogre,
      Demon,
      NumMonsters,
    };

    enum class Action
    {
      Unknown,
      Roaming,
      Hunting,
      Guarding,
      Fighting,
    };

    static const char* TypeToString(Type type);

    Monster(Type monsterType);
    Type GetType() const { return _type; }

    virtual bool IsHuman() const { return false; }

    virtual void DebugDump(vector<string>& dump);
    virtual void DebugDraw(RenderTarget& rt);

    HotloadSprite _sprite;
    Type _type;

    Player* _aggroPlayer;
    Player* _tauntPlayer;
    int _tauntRemaining;
    Pos _lastPlayerPos;
    int _aggroDecay;
    bool _playerVisible;

    int _attackRange;

    int _stunRemaining;
    int _immobilizeRemaining;
    int _courage;
    int _visibilityRange;

    Action _action;
    vector<u8> _visited;
  };

  typedef intrusive_ptr<Monster> MonsterPtr;

  //-----------------------------------------------------------------------------
  inline void intrusive_ptr_add_ref(Monster* e)
  {
    ++e->_refCount;
  }

  //-----------------------------------------------------------------------------
  inline void intrusive_ptr_release(Monster* e)
  {
    if (--e->_refCount == 0)
    {
      delete e;
    }
  }

}
