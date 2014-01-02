#include "monster.hpp"

using namespace rogue;

Monster::Monster()
    : Entity(Type::Monster)
    , _stunRemaining(0)
    , _immobilizeRemaining(0)
    , _action(MonsterAction::kUnknown)
    , _roamStep(0)
    , _retryCount(0)
{
}
