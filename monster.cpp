#include "precompiled.hpp"
#include "monster.hpp"

Monster::Monster() 
  : _maxHealth(3)
  , _health(3)
  , _level(1)
  , _action(MonsterAction::kUnknown)
  , _roamStep(0)
  , _retryCount(0)
{
}
