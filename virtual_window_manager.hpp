#pragma once

namespace rogue
{
  class VirtualWindow;
  class WindowEventManager;

  class VirtualWindowManager
  {
  public:
    typedef function<bool(const Event&)> fnEventHandler;

    VirtualWindowManager(
        RenderWindow* window,
        WindowEventManager* eventManager);
    ~VirtualWindowManager();

    void AddWindow(VirtualWindow* window);
    bool Init();

    void Update();

    // The WindowManager will send the events to the appropriate Virtual Window
    u32 RegisterHandler(Event::EventType event, VirtualWindow* window, const fnEventHandler& handler);
    void UnregisterHandler(u32 handle);

  private:

    bool OnLostFocus(const Event& event);
    bool OnResize(const Event& event);
    bool OnMouseButtonPressed(const Event& event);
    bool OnMouseButtonReleased(const Event& event);
    bool OnMouseMove(const Event& event);
    bool OnKeyReleased(const Event& event);

    bool HandlerForFocusWindow(const Event& event);
    void SetFocus(VirtualWindow* window);

    bool ResetMovingAndResizing();

    typedef pair<u32, fnEventHandler> HandlerPair;
    typedef map<VirtualWindow*, HandlerPair> HandlersByWindow;
    map<Event::EventType, HandlersByWindow > _handlers;

    RenderWindow* _renderWindow;
    WindowEventManager* _eventManager;
    vector<VirtualWindow*> _windows;
    u32 _nextId;
    u32 _nextDepth;

    // Moving data
    Vector2f _startOperationPos;
    Vector2f _windowOrgPos;
    Vector2f _windowOrgSize;
    VirtualWindow* _focusWindow;

    VirtualWindow* _movingWindow;
    VirtualWindow* _resizingWindow;
    u8 _resizeFlags;
  };

}