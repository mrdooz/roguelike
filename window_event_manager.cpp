#include "window_event_manager.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
WindowEventManager::WindowEventManager(RenderWindow* window)
    : _nextId(1)
    , _window(window)
{
}

//-----------------------------------------------------------------------------
size_t WindowEventManager::RegisterHandler(Event::EventType event, const fnEventHandler& handler)
{
  _handlers[event].push_back(make_pair(_nextId, handler));

  // Store the id -> event mapping, so we can speed up unregistering
  _idToEvent[_nextId] = event;
  return _nextId++;
}

//-----------------------------------------------------------------------------
void WindowEventManager::UnregisterHandler(size_t handle)
{
  auto it = _idToEvent.find(handle);
  if (it != _idToEvent.end())
  {
    Event::EventType type = it->second;
    auto& handlers = _handlers[type];
    for (auto j = handlers.begin(); j != handlers.end(); ++j)
    {
      const HandlerPair& p = *j;
      if (p.first == handle)
      {
        handlers.erase(j);
        _idToEvent.erase(handle);
        return;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void WindowEventManager::Poll()
{
  _window->setActive(true);

  // Poll the window for events, and dispatch any to out handlers
  Event event;
  while (_window->pollEvent(event))
  {
    auto it = _handlers.find(event.type);
    if (it != _handlers.end())
    {
      auto& handlers = it->second;
      for (auto p : handlers)
      {
        // Continue calling handlers until one returns true
        if (p.second(event))
          break;
      }
    }
  }
}

