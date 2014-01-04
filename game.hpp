#pragma once

#include "game_state.hpp"
#include "texture_cache.hpp"

namespace rogue
{
  class Party;
  class Renderer;
  class DebugRenderer;
  class EventManager;
  class GameAI;
  class GamePlayer;
  class PlayerFactory;

  class GameEventManager;
  class WindowEventManager;

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

    GameState& GetGameState();
    void SetGameState(const GameState& state);

    WindowEventManager* GetWindowEventManager();
    GameEventManager* GetGameEventManager();

    TextureCache* GetTextureCache();

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

    PlayerFactory* _playerFactory;
    GamePlayer* _gamePlayer;
    GameAI* _gameAI;

    GameState _gameState;

    RenderWindow* _window;
    RenderWindow* _debugWindow;
    Renderer *_renderer;
    DebugRenderer* _debugRenderer;

    WindowEventManager* _windowEventManager;
    GameEventManager* _gameEventManager;

    TextureCache* _textureCache;

    deque<LogMessage> _playerMessages;

    string _appRoot;

    static Game *_instance;
  };

#define GAME rogue::Game::instance()
#define WINDOW_EVENT rogue::Game::instance().GetWindowEventManager()
#define GAME_EVENT rogue::Game::instance().GetGameEventManager()
#define TEXTURE_CACHE rogue::Game::instance().GetTextureCache()

}
