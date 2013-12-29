#include "game_state.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "level.hpp"
#include "party.hpp"
#include "monster.hpp"
#include "game.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
GameState::GameState()
  : _activePlayer(0)
  , _level(nullptr)
  , _party(nullptr)
  , _twoPhaseAction(false)
  , _monsterPhase(false)
{
}

//-----------------------------------------------------------------------------
GameState::~GameState()
{
  delete exch_null(_party);
  delete exch_null(_level);
}

//-----------------------------------------------------------------------------
Player* GameState::GetActivePlayer() const
{
  if (!_party || _activePlayer >= _party->GetNumPlayers())
    return nullptr;

  return _party->_players[_activePlayer];
}
