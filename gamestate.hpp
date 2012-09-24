#ifndef _GAMESTATE_HPP_
#define _GAMESTATE_HPP_

class Level;
class Party;

struct StateBase {

  virtual void update(const sf::Event &event) = 0;
};

struct PlayerState : public StateBase {

  virtual void update(const sf::Event &event) OVERRIDE;

  Level *_level;
  Party *_party;
};


struct AiState : public StateBase {

  virtual void update(const sf::Event &event) OVERRIDE {

  }
};


#endif