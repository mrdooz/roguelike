#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "game_state.hpp"
#include "roguelike.hpp"
#include "event_manager.hpp"

namespace rogue
{
  class Party;
  class Renderer;
  class DebugRenderer;
  class EventManager;
  class GameAI;
  class GamePlayer;

  class Game
  {
  public:

    static Game &instance();
    static bool create();
    static bool close();

    int run();

    void addLogMessage(const char *fmt, ...);

  private:
    Game();
    ~Game();

    bool InitMainWindow();
    bool InitDebugWindow();

    bool OnMouseMove(const Event& event);
    bool OnResized(const Event& event);

    void ProcessMainWindow();
    void ProcessDebugWindow();

    bool init();

    void CreateParty();
    void findAppRoot();

    GamePlayer* _gamePlayer;
    GameAI* _gameAI;

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