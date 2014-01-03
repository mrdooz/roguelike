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

  enum class MonsterAction
  {
    kUnknown,
    kRoaming,
    kGuarding,
    kFighting,
  };


  class Monster : public Entity
  {
  public:
    Monster();

    MonsterType GetMonsterType() const { return _monsterType; }

    sf::Sprite _sprite;
    MonsterType _monsterType;
    vector<Player *> _seenPlayers;
    int _stunRemaining;
    int _immobilizeRemaining;
    int _courage;

    MonsterAction _action;
    vector<Pos> _roamPath;
    size_t _roamStep;
    int _retryCount;
  };
}
