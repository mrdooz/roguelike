#include "game_ai.hpp"
#include "game_state.hpp"
#include "level.hpp"
#include "player.hpp"
#include "monster.hpp"
#include "party.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
void GameAI::Update(GameState& state)
{
  if (!state._monsterPhase)
    return;

  size_t activePlayer = state._activePlayer;
  auto level = state._level;

  for (const auto& m : level->monsters())
  {
    bool done = false;

    // Check if the monster already has seen a player
    if (m->_aggroPlayer)
    {
      // Is the player visible?
      m->_playerVisible = level->IsVisible(m->GetPos(), m->_aggroPlayer->GetPos());
      if (m->_playerVisible)
      {
        m->_lastPlayerPos = m->_aggroPlayer->GetPos();
        m->_aggroDecay = 5;
      }

      // Move towards the last known player position
      if (m->_aggroDecay)
      {
        MoveMonster(level, m, level->StepTowards(m->GetPos(), m->_lastPlayerPos));
        m->_aggroDecay--;
      }
      else
      {
        m->_aggroPlayer = nullptr;
      }
    }
    else
    {
      Pos org(m->GetPos());
      Player* player = nullptr;
      // Search in a spiral for nearby players
      for (int r = 1; r < m->_visibilityRange; ++r)
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
        m->_aggroPlayer = player;
        m->_aggroDecay = 5;
        Pos newPos(level->StepTowards(m->GetPos(), player->GetPos()));
        MoveMonster(level, m, newPos);
      }
      else
      {

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
