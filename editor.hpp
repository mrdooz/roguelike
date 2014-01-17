#pragma once

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

  class Editor
  {
  public:

    static Editor *Instance();
    static bool Create();
    static bool Close();

    int Run();

    WindowEventManager* GetWindowEventManager();
    WindowEventManager* GetDebugWindowEventManager();

    AnimationManager* GetAnimationManager();

  private:
    Editor();
    ~Editor();

    bool InitMainWindow();
    bool InitDebugWindow();

    bool OnMouseMove(const Event& event);
    bool OnResize(const Event& event);

    void ProcessMainWindow();
    void ProcessDebugWindow();

    bool Init();

    void FindAppRoot();

    RenderWindow* _window;
    AnimationEditor* _animationEditor;

    WindowEventManager* _windowEventManager;
    WindowEventManager* _debugWindowEventManager;

    AnimationManager* _animationManager;

    string _appRoot;
    static Editor* _instance;
  };
}
