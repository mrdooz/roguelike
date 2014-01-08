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
    size_t RegisterHandler(Event::EventType event, VirtualWindow* window, const fnEventHandler& handler);
    void UnregisterHandler(size_t handle);

  private:

    bool OnResize(const Event& event);
    bool OnMouseButtonDown(const Event& event);

    bool HandlerForFocusWindow(const Event& event);
    void SetFocus(VirtualWindow* window);

    bool PointOnBorder(const VirtualWindow* window, int x, int y) const;

    typedef pair<size_t, fnEventHandler> HandlerPair;
    typedef map<VirtualWindow*, HandlerPair> HandlersByWindow;
    map<Event::EventType, HandlersByWindow > _handlers;

    RenderWindow* _renderWindow;
    vector<VirtualWindow*> _windows;
    size_t _nextId;

    VirtualWindow* _focusWindow;
  };

}