#pragma once

namespace rogue
{

  class VirtualWindow;

  class VirtualWindowManager
  {
  public:
    typedef function<bool(const Event&)> fnEventHandler;

    VirtualWindowManager(RenderWindow *window);
    ~VirtualWindowManager();

    void AddWindow(VirtualWindow* window);

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

    bool PointOnBorder(const VirtualWindow* window, int x, int y) const;

    typedef pair<u32, fnEventHandler> HandlerPair;
    typedef map<VirtualWindow*, HandlerPair> HandlersByWindow;
    map<Event::EventType, HandlersByWindow > _handlers;

    RenderWindow* _renderWindow;
    vector<VirtualWindow*> _windows;
    u32 _nextId;
    u32 _nextDepth;

    Font _font;

    // Moving data
    Vector2f _startMovePos;
    Vector2f _windowOrgPos;
    VirtualWindow* _focusWindow;

    VirtualWindow* _movingWindow;
  };

}