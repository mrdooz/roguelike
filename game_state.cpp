#include "precompiled.hpp"
#include "game_state.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "level.hpp"
#include "party.hpp"
#include "monster.hpp"
#include "game.hpp"

static bool isTwoPhaseAction(PlayerAction action) {
  return action == PlayerAction::kMeleeAttack || action == PlayerAction::kRangedAttack || action == PlayerAction::kSpellAttack;
}

static bool arrowKeyToOffset(sf::Keyboard::Key code, Pos *ofs) {
  switch (code) {
    case sf::Keyboard::Left:  *ofs = Pos(0,-1); return true;
    case sf::Keyboard::Right: *ofs = Pos(0,+1); return true;
    case sf::Keyboard::Up:    *ofs = Pos(-1,0); return true;
    case sf::Keyboard::Down:  *ofs = Pos(+1,0); return true;
  }
  return false;
}

static bool keyToTwoPhase(sf::Keyboard::Key code, PlayerAction *action) {
  switch (code) {
    case sf::Keyboard::A: *action = PlayerAction::kMeleeAttack; return true;
    case sf::Keyboard::R: *action = PlayerAction::kRangedAttack; return true;
    case sf::Keyboard::S: *action = PlayerAction::kSpellAttack; return true;
  }
  return false;
}

Player *StateBase::playerAt(const Pos &pos) {
  return _level->inside(pos) ? _level->get(pos)._player : nullptr;
}

Monster *StateBase::monsterAt(const Pos &pos) {
  return _level->inside(pos) ? _level->get(pos)._monster : nullptr;
}

void PlayerState::handleAttack(Player *player, Monster *monster) {
  if (0 == --monster->_health) {
    _level->get(monster->_pos)._monster = nullptr;
    GAME.addLogMessage("Player %s killed monster!\n", player->_name.c_str());
  }
}

void PlayerState::update(const sf::Event &event) {

  auto &players = _party->_players;
  Player *player = _party->getActivePlayer();

  if (event.type == sf::Event::KeyReleased) {
    auto code = event.key.code;

    auto &handleMove = [&](sf::Keyboard::Key code) -> bool {
      Pos ofs;
      if (arrowKeyToOffset(code, &ofs)) {
        Pos newPos(player->_pos + ofs);
        if (_level->validDestination(newPos)) {
          player->_hasMoved = true;
          _level->movePlayer(player, player->_pos, newPos);
          player->_pos = newPos;
          return true;
        }
      }
      return false;
    };

    auto &handleTwoPhase = [&](sf::Keyboard::Key code) -> bool {
      auto action = player->_action;

      if (action == PlayerAction::kUnknown) {
        if (keyToTwoPhase(code, &player->_action))
          return true;
      }
      
      // check if we're in the second part of a two phase move
      if (isTwoPhaseAction(action)) {
        Pos ofs;
        if (arrowKeyToOffset(code, &ofs)) {
          if (Monster *monster = monsterAt(player->_pos + ofs)) {
            handleAttack(player, monster);
            player->_hasMoved = true;
            return true;
          }
        }
      }
      return false;
    };

    bool playerMoved = handleTwoPhase(code);

    if (!playerMoved) 
      playerMoved = handleMove(code);

    if (!playerMoved) {
      if (code == sf::Keyboard::Space)  // skip player
        _party->_activePlayer++;
    }
  }

  // Set next player
  bool done = true;
  for (size_t i = 0; i < players.size(); ++i) {
    int idx = (_party->_activePlayer + i) % players.size();
    Player *p = players[idx];
    if (!p->_hasMoved) {
      _party->_activePlayer = idx;
      done = false;
      break;
    }
  }

  if (done) {
    for (Player *player : players) {
      player->_hasMoved = false;
      player->_action = PlayerAction::kUnknown;
    }
    _party->_activePlayer = 0;
  }

}


