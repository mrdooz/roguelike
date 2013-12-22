#include "precompiled.hpp"
#include "game_state.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "level.hpp"
#include "party.hpp"
#include "monster.hpp"
#include "game.hpp"

using namespace rogue;

namespace
{
  static bool isTwoPhaseAction(PlayerAction action)
  {
    return action == PlayerAction::kMeleeAttack
      || action == PlayerAction::kRangedAttack
      || action == PlayerAction::kSpellAttack;
  }

  bool arrowKeyToOffset(sf::Keyboard::Key code, Pos *ofs)
  {
    switch (code)
    {
      case sf::Keyboard::Left:  *ofs = Pos(0,-1); return true;
      case sf::Keyboard::Right: *ofs = Pos(0,+1); return true;
      case sf::Keyboard::Up:    *ofs = Pos(-1,0); return true;
      case sf::Keyboard::Down:  *ofs = Pos(+1,0); return true;
    }
    return false;
  }

  bool keyToTwoPhase(sf::Keyboard::Key code, PlayerAction *action)
  {
    switch (code)
    {
      case sf::Keyboard::A: *action = PlayerAction::kMeleeAttack; return true;
      case sf::Keyboard::R: *action = PlayerAction::kRangedAttack; return true;
      case sf::Keyboard::S: *action = PlayerAction::kSpellAttack; return true;
    }
    return false;
  }
}

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
//-----------------------------------------------------------------------------
void HandleAttack(GameState& state, Player *player, Monster *monster)
{
  if (0 == --monster->_health)
  {
    state._level->monsterKilled(monster);
    GAME.addLogMessage("Player %s killed monster!\n", player->_name.c_str());
  }
}

//-----------------------------------------------------------------------------
void rogue::UpdateState(GameState& state, Event* event)
{
  size_t activePlayer = state._activePlayer;
  auto level = state._level;
  auto party = state._party;

  bool validMove = false;

  if (event)
  {
    if (event->type == sf::Event::KeyReleased)
    {
      if (!state._monsterPhase)
      {
        Keyboard::Key key = event->key.code;
        // Tick the active player
        auto player = party->_players[state._activePlayer];
        if (state._twoPhaseAction)
        {
          Pos ofs;
          if (arrowKeyToOffset(key, &ofs))
          {
            if (Monster *monster = level->monsterAt(player->_pos + ofs))
            {
              HandleAttack(state, player, monster);
              player->_hasMoved = true;
              validMove = true;
            }
          }
        }
        else
        {
          // Check if the input is a valid movement key, and the
          // resulting position is valid
          Pos ofs;
          if (arrowKeyToOffset(key, &ofs))
          {
            Pos newPos(player->_pos + ofs);
            if (state._level->validDestination(newPos))
            {
              player->_hasMoved = true;
              state._level->movePlayer(player, player->_pos, newPos);
              player->_pos = newPos;
              validMove = true;
            }
          }
          else if (key == sf::Keyboard::Space)
          {
            // skip player
            player->_hasMoved = true;
            validMove = true;
          }
        }
        activePlayer++;
      }
    }
    else if (event->type == sf::Event::MouseMoved)
    {

    }
    else if (event->type == sf::Event::MouseButtonPressed)
    {

    }
  }
  else
  {
    // no event
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
          Pos rnd(monsters[rand() % cnt]->_pos);
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
