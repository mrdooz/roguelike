#pragma once

#include "entity.hpp"

namespace rogue
{
  class Player;

  class Monster : public Entity
  {
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

    sf::Sprite _sprite;
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
}
