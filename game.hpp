#pragma once

#include "game_state.hpp"

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

  class TextureCache;
  class AnimationManager;

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

    GameState& GetGameState();

    WindowEventManager* GetWindowEventManager();
    WindowEventManager* GetDebugWindowEventManager();
    GameEventManager* GetGameEventManager();

    TextureCache* GetTextureCache();
    AnimationManager* GetAnimationManager();

  private:
    Game();
    ~Game();

    bool InitMainWindow();
    bool InitDebugWindow();

    bool OnMouseMove(const Event& event);
    bool OnResize(const Event& event);

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
    WindowEventManager* _debugWindowEventManager;
    GameEventManager* _gameEventManager;

    TextureCache* _textureCache;
    AnimationManager* _animationManager;

    deque<LogMessage> _playerMessages;

    string _appRoot;

    static Game *_instance;
  };

#define GAME rogue::Game::instance()
#define WINDOW_EVENT rogue::Game::instance().GetWindowEventManager()
#define DEBUG_WINDOW_EVENT rogue::Game::instance().GetDebugWindowEventManager()
#define GAME_EVENT rogue::Game::instance().GetGameEventManager()
#define TEXTURE_CACHE rogue::Game::instance().GetTextureCache()
#define ANIMATION rogue::Game::instance().GetAnimationManager()

}
