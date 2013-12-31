#include "monster.hpp"

using namespace rogue;

Monster::Monster() 
  : _maxHealth(3)
  , _health(3)
  , _level(1)
  , _stunRemaining(0)
  , _immobilizeRemaining(0)
  , _action(MonsterAction::kUnknown)
  , _roamStep(0)
  , _retryCount(0)
{
}
