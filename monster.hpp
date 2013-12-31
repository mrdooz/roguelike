#pragma once

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


  class Monster
  {
  public:
    Monster();
    sf::Sprite _sprite;
    MonsterType _type;
    Pos _pos;
    int _maxHealth;
    int _health;
    int _level;
    vector<Player *> _seenPlayers;
    int _stunRemaining;
    int _immobilizeRemaining;

    MonsterAction _action;
    vector<Pos> _roamPath;
    size_t _roamStep;
    int _retryCount;
  };
}
