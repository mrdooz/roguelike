#include "spell.hpp"
#include "game_state.hpp"
#include "party.hpp"
#include "player.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "game_event_manager.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
bool SpellBase::OnMonsterSelected(GameState& state, Monster* monster)
{
  return true;
}

//-----------------------------------------------------------------------------
bool SpellBase::OnPlayerSelected(GameState& state, Player* player)
{
  return true;
}

//-----------------------------------------------------------------------------
bool SpellBase::Finished(const GameState& state)
{
  return true;
}

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
    for (int i = 0; i < player->_chargeRange; ++i)
    {
      Pos newPos(player->GetPos() + (int)i * ofs);
      auto& tile = level->Get(newPos);
      if (tile._type != Tile::Type::Floor || tile._player)
        break;

      if (tile._type == Tile::Type::Floor && tile._monster)
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
bool SpellArcaneBlast::Finished(const GameState& state)
{
  return state._actionPhase >= 2;
}

//-----------------------------------------------------------------------------
bool SpellArcaneBlast::OnMonsterSelected(GameState& state, Monster* monster)
{
  GameEvent event(GameEvent::Type::Attack);
  Player* player = state.GetActivePlayer();
  event._agent = player;
  event._target = monster;

  event._damage = (5 * player->Level() + player->WeaponBonus());

  GAME_EVENT->SendEvent(event);
  return true;
}

//-----------------------------------------------------------------------------
bool SpellArcaneBlast::IsValid(GameState& state, const Event& event)
{
  // Check if the current target is a monster within range
  if (event.type == Event::MouseButtonReleased)
  {

  }

  return false;
}

//-----------------------------------------------------------------------------
bool SpellPoisonCloud::IsValid(GameState& state, const Event& event)
{
  return false;
}
