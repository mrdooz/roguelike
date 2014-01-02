#pragma once

namespace rogue
{
  class WindowEventManager
  {
  public:
    typedef function<bool(const Event&)> fnEventHandler;

    WindowEventManager(RenderWindow* window);

    size_t RegisterHandler(Event::EventType event, const fnEventHandler& handler);
    void UnregisterHandler(size_t handle);

    void Poll();

  private:
    typedef pair<size_t, fnEventHandler> HandlerPair;
    map<Event::EventType, vector<HandlerPair> > m_handlers;
    unordered_map<size_t, Event::EventType> m_IdToEvent;
    size_t m_nextId;

    RenderWindow* _window;
  };
}
