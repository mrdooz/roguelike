#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "gamestate.hpp"

class Party;
class Renderer;

class Game {

public:

  bool close();
  bool init();
  void update(const sf::Event &event);
  int run();

private:

  void findAppRoot();

  Level *_level;
  Party *_party;

  PlayerState _playerState;
  AiState _aiState;
  StateBase *_curState;

  sf::RenderWindow *_window;
  Renderer *_renderer;

  std::string _appRoot;
};

#endif