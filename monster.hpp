#ifndef _MONSTER_HPP_
#define _MONSTER_HPP_

#include "types.hpp"

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
  std::vector<Player *> _seenPlayers;

  MonsterAction _action;
  std::vector<Pos> _roamPath;
  size_t _roamStep;
  int _retryCount;
};

#endif