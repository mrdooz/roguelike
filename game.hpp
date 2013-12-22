#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "game_state.hpp"
#include "roguelike.hpp"

namespace rogue
{
  class Party;
  class Renderer;
  class DebugRenderer;
  class EventManager;

  class Game
  {
  public:

    static Game &instance();
    static bool create();
    static bool close();

    int run();

    void addLogMessage(const char *fmt, ...);

  private:
    ~Game();

    void ProcessMainWindow();
    void ProcessDebugWindow();

    bool init();

    void CreateParty();
    void findAppRoot();

    GameState _gameState;

    RenderWindow* _window;
    RenderWindow* _debugWindow;
    Renderer *_renderer;
    DebugRenderer* _debugRenderer;

    EventManager* _eventManager;

    string _appRoot;

    static Game *_instance;
  };

#define GAME rogue::Game::instance()


}

#endif