#pragma once

#include "game_state.hpp"
#include "event_manager.hpp"

namespace rogue
{
  class Party;
  class Renderer;
  class DebugRenderer;
  class EventManager;
  class GameAI;
  class GamePlayer;

  struct LogMessage
  {
    LogMessage(const ptime& expiry, const string& message) : _expiration(expiry), _message(message) {}
    ptime _expiration;
    string _message;
  };

  class Game
  {
  public:

    static Game &instance();
    static bool create();
    static bool close();

    int run();

    void AddPlayerMessage(const time_duration& duration, const char* fmt, ...);
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
    void FindAppRoot();

    void ProcessPlayerMessages(const ptime& now);

    Font _playerMessageFont;

    GamePlayer* _gamePlayer;
    GameAI* _gameAI;

    GameState _gameState;

    RenderWindow* _window;
    RenderWindow* _debugWindow;
    Renderer *_renderer;
    DebugRenderer* _debugRenderer;

    EventManager* _eventManager;

    deque<LogMessage> _playerMessages;

    string _appRoot;

    static Game *_instance;
  };

#define GAME rogue::Game::instance()

}
