#ifndef _MONSTER_HPP_
#define _MONSTER_HPP_

#include "types.hpp"

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


class Monster {
public:
  Monster();
  sf::Sprite _sprite;
  MonsterType _type;
  int _level;
  int _maxHealth;
  int _health;
  Pos _pos;
};

#endif