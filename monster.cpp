#include "monster.hpp"
#include "utils.hpp"

using namespace rogue;

Monster::Monster()
    : Entity(Type::Monster)
    , _aggroPlayer(nullptr)
    , _stunRemaining(0)
    , _immobilizeRemaining(0)
    , _action(Action::kUnknown)
    , _visibilityRange(5)
    , _aggroDecay(0)
{
}

void Monster::DebugDump(vector<string>& dump)
{
  dump.push_back(toString("Aggro: 0x%.8x", _aggroPlayer));
  dump.push_back(toString("AggroDecay: %d", _aggroDecay));
}
