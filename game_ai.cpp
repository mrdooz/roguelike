#include "game_ai.hpp"
#include "game_state.hpp"
#include "level.hpp"
#include "player.hpp"
#include "monster.hpp"
#include "party.hpp"
#include "game.hpp"
#include "game_event_manager.hpp"
#include "utils.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
void GameAI::AttackPlayer(Monster* monster, Player* player)
{
  GameEvent event(GameEvent::Type::Attack);
  event._agent = monster;
  event._target = player;
  event._damage = monster->Level() + (rand() % monster->Level());
  GAME_EVENT->SendEvent(event);
}

//-----------------------------------------------------------------------------
void GameAI::Update(GameState& state)
{
  if (!state._monsterPhase)
    return;

  auto level = state._level;

  for (const auto& monster : level->monsters())
  {
    Player* player = monster->_aggroPlayer;
    // Check if the monster already has seen a player
    if (player)
    {
      // Check if the player is adjacent
      if (MDist(player->GetPos(), monster->GetPos()) == 1)
      {
        AttackPlayer(monster, player);
      }
      else
      {
        // Is the player visible?
        monster->_playerVisible = level->IsVisible(monster->GetPos(), monster->_aggroPlayer->GetPos());
        if (monster->_playerVisible)
        {
          monster->_lastPlayerPos = monster->_aggroPlayer->GetPos();
          monster->_aggroDecay = 5;
        }

        // Move towards the last known player position
        if (monster->_aggroDecay)
        {
          MoveMonster(level, monster, level->StepTowards(monster->GetPos(), monster->_lastPlayerPos));
          monster->_aggroDecay--;
          monster->_playerVisible = level->IsVisible(monster->GetPos(), monster->_aggroPlayer->GetPos());
          if (monster->_playerVisible)
          {
            monster->_lastPlayerPos = monster->_aggroPlayer->GetPos();
            monster->_aggroDecay = 5;
          }
        }
        else
        {
          monster->_aggroPlayer = nullptr;
        }
      }
    }
    else
    {
      Pos org(monster->GetPos());
      Player* player = nullptr;
      // Search in a spiral for nearby players
      for (int r = 1; r < monster->_visibilityRange; ++r)
      {
        // top
        for (int i = -r; i <= r; ++i)
        {
          Pos p = org + Pos(i, r);
          if (level->Inside(p) && level->IsVisible(org, p) && level->Get(p)._player)
          {
            player = level->Get(p)._player;
            goto FOUND_PLAYER;
          }
        }

        // right
        for (int i = -r+1; i <= r-1; ++i)
        {
          Pos p = org + Pos(r, i);
          if (level->Inside(p) && level->IsVisible(org, p) && level->Get(p)._player)
          {
            player = level->Get(p)._player;
            goto FOUND_PLAYER;
          }
        }

        // bottom
        for (int i = -r; i <= r; ++i)
        {
          Pos p = org + Pos(i, -r);
          if (level->Inside(p) && level->IsVisible(org, p) && level->Get(p)._player)
          {
            player = level->Get(p)._player;
            goto FOUND_PLAYER;
          }
        }

        // left
        for (int i = -r+1; i <= r-1; ++i)
        {
          Pos p = org + Pos(-r, i);
          if (level->Inside(p) && level->IsVisible(org, p) && level->Get(p)._player)
          {
            player = level->Get(p)._player;
            goto FOUND_PLAYER;
          }
        }
      }
FOUND_PLAYER:
      if (player)
      {
        monster->_aggroPlayer = player;
        monster->_aggroDecay = 5;
        monster->_playerVisible = true;
        Pos newPos(level->StepTowards(monster->GetPos(), player->GetPos()));
        MoveMonster(level, monster, newPos);
      }
      else
      {
        // if no player is close, roam towards the nearest unexplored part

      }
    }
  }

  state._monsterPhase = false;
}

//-----------------------------------------------------------------------------
void GameAI::MoveMonster(Level* level, Monster* monster, const Pos& newPos)
{
  level->MoveMonster(monster, newPos);
  monster->SetPos(newPos);
}
