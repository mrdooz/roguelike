#ifndef _GAME_STATE_HPP_
#define _GAME_STATE_HPP_

#include "types.hpp"

class Level;
class Party;

class Player;
class Monster;

struct StateBase {

  virtual void update(const sf::Event &event) = 0;

  Player *playerAt(const Pos &pos);
  Monster *monsterAt(const Pos &pos);
  Level *_level;
  Party *_party;

};

struct PlayerState : public StateBase {

  virtual void update(const sf::Event &event) OVERRIDE;

  void handleAttack(Player *player, Monster *monster);

};


struct AiState : public StateBase {

  virtual void update(const sf::Event &event) OVERRIDE {

  }
};


#endif