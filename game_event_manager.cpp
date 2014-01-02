#include "game_event_manager.hpp"
#include "error.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
GameEventManager::GameEventManager()
    : m_nextId(1)
{
}

//-----------------------------------------------------------------------------
size_t GameEventManager::RegisterHandler(GameEvent::Type event, const fnEventHandler& handler)
{
  m_handlers[event].push_back(make_pair(m_nextId, handler));

  // Store the id -> event mapping, so we can speed up unregistering
  m_IdToEvent[m_nextId] = event;
  return m_nextId++;
}

//-----------------------------------------------------------------------------
void GameEventManager::UnregisterHandler(size_t handle)
{
  auto it = m_IdToEvent.find(handle);
  if (it != m_IdToEvent.end())
  {
    GameEvent::Type type = it->second;
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
void GameEventManager::SendEvent(const GameEvent& event)
{
  auto it = m_handlers.find(event._type);
  if (it == m_handlers.end())
  {
//    LOG_DEBUG(LogKeyValue("event", "no handlers registered for event")
//           << LogKeyValue("type", (int)event.type));
    return;
  }

  // Send event to all listeners
  for (auto& h : it->second)
  {
    h.second(event);
  }
}
