#pragma once

#include "game_state.hpp"
#include "selection.hpp"

namespace rogue
{
  class Party;
  class Renderer;
  class EventManager;
  class GameAI;
  class GamePlayer;
  class PlayerFactory;

  class GameEventManager;
  class WindowEventManager;

  class TextureCache;
  class AnimationManager;
  class AnimationEditor;

  struct LogMessage
  {
    LogMessage(const ptime& expiry, const string& message) : _expiration(expiry), _message(message) {}
    ptime _expiration;
    string _message;
  };

  class Game
  {
  public:
    typedef function<void(const SelectionEvent&)> fnSelectionListener;

    static Game *Instance();
    static bool Create();
    static bool Close();

    int Run();

    void AddPlayerMessage(const time_duration& duration, const char* fmt, ...);
    void AddPlayerMessage(const char* fmt, ...);

    GameState& GetGameState();

    WindowEventManager* GetWindowEventManager();
    WindowEventManager* GetDebugWindowEventManager();
    GameEventManager* GetGameEventManager();

    TextureCache* GetTextureCache();
    AnimationManager* GetAnimationManager();

    u32 RegisterSelectionListener(const fnSelectionListener& listener);
    void UnregisterSelectionListener(u32 id);
    void PostSelectionEvent(const SelectionEvent& event);

  private:
    Game();
    ~Game();

    bool InitMainWindow();
    bool InitDebugWindow();

    bool OnMouseMove(const Event& event);
    bool OnResize(const Event& event);

    void ProcessMainWindow();
    void ProcessDebugWindow();

    bool Init();

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
    AnimationEditor* _animationEditor;

    WindowEventManager* _windowEventManager;
    WindowEventManager* _debugWindowEventManager;
    GameEventManager* _gameEventManager;

    TextureCache* _textureCache;
    AnimationManager* _animationManager;

    deque<LogMessage> _playerMessages;

    string _appRoot;

    map<u32, fnSelectionListener> _selectionListeners;
    u32 _nextSelectionId;

    static Game *_instance;
  };

#define GAME rogue::Game::Instance()
#define GAME_EVENT rogue::Game::Instance()->GetGameEventManager()
#define TEXTURE_CACHE rogue::Game::Instance()->GetTextureCache()
#define ANIMATION rogue::Game::Instance()->GetAnimationManager()

}
