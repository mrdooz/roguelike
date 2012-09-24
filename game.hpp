#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "game_state.hpp"

class Party;
class Renderer;

class Game {

public:

  static Game &instance();
  static bool create();
  static bool close();

  void update(const sf::Event &event);
  int run();

  void addLogMessage(const char *fmt, ...);

private:
  ~Game();
  bool init();

  void findAppRoot();

  Level *_level;
  Party *_party;

  PlayerState _playerState;
  AiState _aiState;
  StateBase *_curState;

  sf::RenderWindow *_window;
  Renderer *_renderer;

  std::string _appRoot;

  static Game *_instance;
};

#define GAME Game::instance()

#endif