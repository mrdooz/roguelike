#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "gamestate.hpp"

class Party;

class Game {

public:

  bool close();
  bool init();
  void update(const sf::Event &event);
  int run();

private:

  void drawPartyStats(int width);
  void findAppRoot();

  Party *_party;
  PlayerState _playerState;
  AiState _aiState;

  StateBase *_curState;

  Level *_level;

  sf::Font _font;

  sf::Texture _environmentTexture;
  sf::Texture _characterTexture;
  sf::RenderWindow *_window;

  std::string _appRoot;

};

#endif