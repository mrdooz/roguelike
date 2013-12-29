#include "game_ai.hpp"
#include "game_state.hpp"
#include "level.hpp"
#include "player.hpp"
#include "monster.hpp"
#include "party.hpp"

using namespace rogue;

void GameAI::Update(GameState& state)
{
  size_t activePlayer = state._activePlayer;
  auto level = state._level;
  bool validMove = false;

  if (state._monsterPhase)
  {
    validMove = true;

    // Monster AI
    state._monsterPhase = false;

    auto& monsters = level->monsters();
    size_t cnt = monsters.size();
    for (size_t i = 0; i < cnt; ++i)
    {
      auto *monster = monsters[i];
      if (monster->_action == MonsterAction::kUnknown)
      {
        // move towards the active player
        Pos dest(state.GetActivePlayer()->_pos);
        //Pos rnd(monsters[rand() % cnt]->_pos);
        if (level->calcPath(monster->_pos, dest, &monster->_roamPath))
        {
          monster->_action = MonsterAction::kRoaming;
          monster->_roamStep = 1;
        }
      }

      if (monster->_action == MonsterAction::kRoaming)
      {
        bool calcNewPath = monster->_roamStep >= monster->_roamPath.size();
        if (!calcNewPath)
        {
          Pos nextPos = monster->_roamPath[monster->_roamStep];
          if (level->tileFree(nextPos)) {
            monster->_retryCount = 0;
            level->moveMonster(monster, monster->_pos, nextPos);
            monster->_pos = nextPos;
            monster->_roamStep++;
          } else {
            if (++monster->_retryCount > 3) {
              calcNewPath = true;
            }
          }
        }

        if (calcNewPath)
        {
          if (level->calcPath(monster->_pos, monsters[rand() % cnt]->_pos, &monster->_roamPath))
          {
            monster->_action = MonsterAction::kRoaming;
            monster->_roamStep = 1;
            monster->_retryCount = 0;
          }
          else
          {
            monster->_action = MonsterAction::kUnknown;
          }
        }
      }
    }
  }

  if (validMove)
  {
    if (activePlayer == state._party->GetNumPlayers())
    {
      state._activePlayer = 0;
      state._monsterPhase = true;
    }
    else
    {
      state._activePlayer = activePlayer;
    }
  }

}