#ifndef _MONSTER_HPP_
#define _MONSTER_HPP_

#include "types.hpp"
#include "roguelike.hpp"

namespace rogue
{
  class Player;

  enum class MonsterType {
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

  enum class MonsterAction {
    kUnknown,
    kRoaming,
    kGuarding,
    kFighting,
  };


  class Monster {
  public:
    Monster();
    sf::Sprite _sprite;
    MonsterType _type;
    int _level;
    int _maxHealth;
    int _health;
    Pos _pos;
    vector<Player *> _seenPlayers;

    MonsterAction _action;
    vector<Pos> _roamPath;
    size_t _roamStep;
    int _retryCount;
  };
}

#endif