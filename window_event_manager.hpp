#pragma once

namespace rogue
{
  class WindowEventManager
  {
  public:
    typedef function<bool(const Event&)> fnEventHandler;

    WindowEventManager(RenderWindow* window);

    u32 RegisterHandler(Event::EventType event, const fnEventHandler& handler);
    void UnregisterHandler(u32 handle);

    void Poll();

  private:
    typedef pair<u32, fnEventHandler> HandlerPair;
    map<Event::EventType, vector<HandlerPair> > _handlersByWindow;
    unordered_map<u32, Event::EventType> _idToEvent;
    u32 _nextId;

    RenderWindow* _window;
  };
}
