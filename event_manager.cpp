#include "precompiled.hpp"
#include "event_manager.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
EventManager::EventManager(RenderWindow* window)
    : m_nextId(1)
    , _window(window)
{
}

//-----------------------------------------------------------------------------
size_t EventManager::RegisterHandler(Event::EventType event, const fnEventHandler& handler)
{
  m_handlers[event].push_back(make_pair(m_nextId, handler));

  // Store the id -> event mapping, so we can speed up unregistering
  m_IdToEvent[m_nextId] = event;
  return m_nextId++;
}

//-----------------------------------------------------------------------------
void EventManager::UnregisterHandler(size_t handle)
{
  auto it = m_IdToEvent.find(handle);
  if (it != m_IdToEvent.end())
  {
    Event::EventType type = it->second;
    auto& handlers = m_handlers[type];
    for (auto j = handlers.begin(); j != handlers.end(); ++j)
    {
      const HandlerPair& p = *j;
      if (p.first == handle)
      {
        handlers.erase(j);
        m_IdToEvent.erase(handle);
        return;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void EventManager::Update()
{
  _window->setActive(true);

  // Poll the window for events, and dispatch any to out handlers
  Event event;
  while (_window->pollEvent(event))
  {
    auto it = m_handlers.find(event.type);
    if (it != m_handlers.end())
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

