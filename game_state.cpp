#include "game_state.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "level.hpp"
#include "party.hpp"
#include "monster.hpp"
#include "game.hpp"
#include "player_factory.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
GameState::GameState()
  : _elapsedTime(0)
  , _activePlayer(0)
  , _level(nullptr)
  , _party(nullptr)
  , _actionPhase(0)
  , _selection(0)
  , _curSpell(nullptr)
  , _monsterPhase(false)
  , _playerInProgress(false)
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

  return _party->_players[_activePlayer].get();
}

//-----------------------------------------------------------------------------
void GameState::CreateParty(PlayerFactory* playerFactory)
{
  _party = new Party();

  for (int i = 0; i < 4; ++i)
  {
    auto *p = playerFactory->CreatePlayer((PlayerClass)i, toString("Player %d", i));
    while (true)
    {
      int x = rand() % _level->Width();
      int y = rand() % _level->Height();
      auto& tile = _level->Get(x, y);
      if (tile.IsEmpty(false) && tile._type == Tile::Type::Floor)
      {
        p->SetPos(Pos(x, y));
        break;
      }
    }
    _level->initPlayer(p, p->GetPos());
    _party->_players.push_back(p);
  }
}

