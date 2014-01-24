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
bool SpellBase::IsValid(GameState& state, const Event& event)
{
  return true;
}

//-----------------------------------------------------------------------------
bool SpellBase::OnMonsterSelected(GameState& state, Monster* monster)
{
  return false;
}

//-----------------------------------------------------------------------------
bool SpellBase::OnPlayerSelected(GameState& state, Player* player)
{
  return false;
}

//-----------------------------------------------------------------------------
bool SpellBase::Finished(const GameState& state)
{
  return true;
}

//-----------------------------------------------------------------------------
Animation::Id SpellBase::AnimationId()
{
  return Animation::Id::None;
}

//-----------------------------------------------------------------------------
bool SpellCharge::IsValid(GameState& state, const Event& event)
{
  auto party = state._party;
  auto player = party->_players[state._activePlayer];
  auto level = state._level;

  // for a charge to be valid, there must be a free path to a mob in the given
  // direction
  MonsterPtr monster;
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
    GAME->AddPlayerMessage(seconds(3), "Invalid charge");
  }

  return !!monster;
}

//-----------------------------------------------------------------------------
bool SpellArcaneBlast::Finished(const GameState& state)
{
  return state._actionPhase >= 2;
}

//-----------------------------------------------------------------------------
Animation::Id SpellArcaneBlast::AnimationId()
{
  return Animation::Id::ArcaneBlast;
}

//-----------------------------------------------------------------------------
bool SpellArcaneBlast::OnMonsterSelected(GameState& state, Monster* monster)
{
  Player* player = state.GetActivePlayer();
  if (!state._level->IsVisible(player->GetPos(), monster->GetPos()))
    return false;

  GameEvent event(GameEvent::Type::Attack);
  event._agent = player;
  event._target = monster;
  event._spell = this;

  event._damage = (5 * player->Level() + player->WeaponBonus());

  GAME_EVENT->SendEvent(event);
  return true;
}

//-----------------------------------------------------------------------------
bool SpellLightningBolt::OnMonsterSelected(GameState& state, Monster* monster)
{
  Player* player = state.GetActivePlayer();
  if (!state._level->IsVisible(player->GetPos(), monster->GetPos()))
    return false;

  // Zap all mobs in the LOS (skip self!)
  vector<Entity*> mobs;
  state._level->EntitiesInPath(player->GetPos(), monster->GetPos(), &mobs);

  for (size_t i = 1; i < mobs.size(); ++i)
  {
    GameEvent event(GameEvent::Type::Attack);
    event._agent = player;
    event._target = mobs[i];
    event._spell = this;

    event._damage = (5 * player->Level() + player->WeaponBonus());

    GAME_EVENT->SendEvent(event);
  }

  return true;
}

//-----------------------------------------------------------------------------
Animation::Id SpellLightningBolt::AnimationId()
{
  return Animation::Id::LightningBolt2;
}
