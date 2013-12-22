#include "precompiled.hpp"
#include "party.hpp"
#include "player.hpp"
#include "utils.hpp"

using namespace rogue;

size_t Party::GetNumPlayers() const
{
  return _players.size();
}
