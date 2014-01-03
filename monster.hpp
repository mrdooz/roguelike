#pragma once

#include "entity.hpp"

namespace rogue
{
  class Player;

  enum class MonsterType
  {
    kGoblin,
    kSkeleton,
    kSkeletonWarrior,
    kSkeletonMage,
    kFireElemental,
    kWaterElemental,
    kOgre,
    kDemon,
    cNumMonsters,
  };

  class Monster : public Entity
  {
  public:
    enum class Action
    {
      kUnknown,
      kRoaming,
      kHunting,
      kGuarding,
      kFighting,
    };

    Monster();
    MonsterType GetMonsterType() const { return _monsterType; }

    virtual void DebugDump(vector<string>& dump);
    virtual void DebugDraw(RenderTarget& rt);

    sf::Sprite _sprite;
    MonsterType _monsterType;

    Player* _aggroPlayer;
    Pos _lastPlayerPos;
    int _aggroDecay;
    bool _playerVisible;

    int _stunRemaining;
    int _immobilizeRemaining;
    int _courage;
    int _visibilityRange;

    Action _action;
    vector<u8> _visited;
  };
}
