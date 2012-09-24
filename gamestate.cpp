#include "precompiled.hpp"
#include "gamestate.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "level.hpp"

void PlayerState::update(const sf::Event &event) {

  auto &players = _party->_players;
  Player *player = players[_party->_activePlayer];
  bool movePlayer = false;
  bool skipPlayer = false;

  Pos newPos;
  Pos oldPos = player->_pos;

  if (event.type == sf::Event::KeyReleased) {
    auto code = event.key.code;
    struct {
      sf::Keyboard::Key code;
      Pos ofs;
    } moves[] = {
      { sf::Keyboard::Left,   Pos(0,-1) },
      { sf::Keyboard::Right,  Pos(0,+1) },
      { sf::Keyboard::Up,     Pos(-1,0) },
      { sf::Keyboard::Down,   Pos(+1,0) },
    };

    for (int i = 0; i < ELEMS_IN_ARRAY(moves); ++i) {
      auto &cur = moves[i];
      newPos = Pos(player->_pos + cur.ofs);
      if (code == cur.code) {
        if (_level->validDestination(newPos)) {
          movePlayer = true;
          break;
        }
      }
    }

    if (!movePlayer) {
      if (code == sf::Keyboard::Space) {
        skipPlayer = true;
      }
    }
  }

  if (movePlayer) {
    player->_hasMoved = true;
    //player->_activePlayer = false;
    _level->movePlayer(player, oldPos, newPos);
    player->_pos = newPos;
  }

  if (skipPlayer) {
    //player->_activePlayer = false;
    _party->_activePlayer++;
  }

  // Set next player
  bool done = true;
  for (size_t i = 0; i < players.size(); ++i) {
    int idx = (_party->_activePlayer + i) % players.size();
    Player *p = players[idx];
    if (!p->_hasMoved) {
      _party->_activePlayer = idx;
      //p->_activePlayer = true;
      done = false;
      break;
    }
  }

  if (done) {
    for (size_t i = 0; i < players.size(); ++i) {
      //players[i]->_activePlayer = false;
      players[i]->_hasMoved = false;
    }
    _party->_activePlayer = 0;
    //players[0]->_activePlayer = true;
  }

}


