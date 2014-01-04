#include "game_event_manager.hpp"
#include "error.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
GameEventManager::GameEventManager()
    : _nextId(1)
{
}

//-----------------------------------------------------------------------------
size_t GameEventManager::RegisterHandler(GameEvent::Type event, const fnEventHandler& handler)
{
  _handlers[event].push_back(make_pair(_nextId, handler));

  // Store the id -> event mapping, so we can speed up unregistering
  _idToEvent[_nextId] = event;
  return _nextId++;
}

//-----------------------------------------------------------------------------
void GameEventManager::UnregisterHandler(size_t handle)
{
  auto it = _idToEvent.find(handle);
  if (it != _idToEvent.end())
  {
    GameEvent::Type type = it->second;
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
void GameEventManager::SendEvent(const GameEvent& event)
{
  auto it = _handlers.find(event._type);
  if (it == _handlers.end())
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
