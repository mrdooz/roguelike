#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "game_state.hpp"
#include "roguelike.hpp"

namespace rogue
{
  class Party;
  class Renderer;

  class Game {

  public:

    static Game &instance();
    static bool create();
    static bool close();

    int run();

    void addLogMessage(const char *fmt, ...);

  private:
    ~Game();
    bool init();

    void findAppRoot();
    void handleNextState(GameState nextState);

    Level *_level;
    Party *_party;

    PlayerState _playerState;
    AiState _aiState;
    StateBase *_curState;

    sf::RenderWindow *_window;
    Renderer *_renderer;

    string _appRoot;

    static Game *_instance;
  };

#define GAME rogue::Game::instance()


}

#endif