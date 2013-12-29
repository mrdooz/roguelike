#include "game_player.hpp"
#include "party.hpp"
#include "level.hpp"
#include "player.hpp"
#include "monster.hpp"

using namespace rogue;

namespace
{
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

#if 0
  static bool isTwoPhaseAction(PlayerAction action)
  {
    return action == PlayerAction::kMeleeAttack
    || action == PlayerAction::kRangedAttack
    || action == PlayerAction::kSpellAttack;
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
#endif
  //-----------------------------------------------------------------------------
  void HandleAttack(GameState& state, Player *player, Monster *monster)
  {
    if (0 == --monster->_health)
    {
      state._level->monsterKilled(monster);
//      GAME.addLogMessage("Player %s killed monster!\n", player->_name.c_str());
    }
  }

}

//-----------------------------------------------------------------------------
void GamePlayer::Update(GameState& gameState)
{
}

//-----------------------------------------------------------------------------
bool GamePlayer::OnKeyPressed(GameState& state, const Event& event)
{
  if (state._monsterPhase)
    return false;

  size_t activePlayer = state._activePlayer;
  auto level = state._level;
  auto party = state._party;

  bool validMove = false;

  Keyboard::Key key = event.key.code;
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

  return true;
}

