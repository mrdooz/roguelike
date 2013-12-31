#include "spell.hpp"
#include "game_state.hpp"
#include "party.hpp"
#include "player.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "game.hpp"
#include "utils.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
bool SpellCharge::IsValid(GameState& state, const Event& event)
{
  auto party = state._party;
  auto player = party->_players[state._activePlayer];
  auto level = state._level;

  // for a charge to be valid, there must be a free path to a mob in the given
  // direction
  Monster* monster = nullptr;
  Pos ofs;
  if (ArrowKeyToOffset(event.key.code, &ofs))
  {
    for (size_t i = 0; i < player->_chargeRange; ++i)
    {
      Pos newPos(player->_pos + (int)i * ofs);
      auto& tile = level->Get(newPos);
      if (tile._type != TileType::kFloor || tile._player)
        break;

      if (tile._type == TileType::kFloor && tile._monster)
      {
        monster = tile._monster;
        break;
      }
    }
  }

  if (monster)
  {
    monster->_stunRemaining += 3;
  }
  else
  {
    GAME.AddPlayerMessage(seconds(3), "Invalid charge");
  }

  return !!monster;
}

//-----------------------------------------------------------------------------
bool SpellMightyBlow::IsValid(GameState& state, const Event& event)
{
  return false;
}

//-----------------------------------------------------------------------------
bool SpellLightningBolt::IsValid(GameState& state, const Event& event)
{
  return false;
}

//-----------------------------------------------------------------------------
bool SpellFireball::IsValid(GameState& state, const Event& event)
{
  return false;
}

//-----------------------------------------------------------------------------
bool SpellArcaneBlast::IsValid(GameState& state, const Event& event)
{
  return false;
}

//-----------------------------------------------------------------------------
bool SpellPoisonCloud::IsValid(GameState& state, const Event& event)
{
  return false;
}
