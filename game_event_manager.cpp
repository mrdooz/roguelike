#include "game_event_manager.hpp"
#include "error.hpp"
#include "loot_item.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
GameEvent::GameEvent(Type type)
  : _type(type)
{
}

//-----------------------------------------------------------------------------
GameEvent::GameEvent(Entity* agent, Entity* target, LootItem* item)
  : _type(Type::ItemGained)
  , _agent(agent)
  , _target(target)
  , _item(item)
{
}

//-----------------------------------------------------------------------------
GameEvent::~GameEvent()
{
  if (_type == Type::ItemGained)
    delete exch_null(_item);
}

//-----------------------------------------------------------------------------
GameEventManager::GameEventManager()
    : _nextId(1)
{
}

//-----------------------------------------------------------------------------
u32 GameEventManager::RegisterHandler(GameEvent::Type event, const fnEventHandler& handler)
{
  _handlersByWindow[event].push_back(make_pair(_nextId, handler));

  // Store the id -> event mapping, so we can speed up unregistering
  _idToEvent[_nextId] = event;
  return _nextId++;
}

//-----------------------------------------------------------------------------
void GameEventManager::UnregisterHandler(u32 handle)
{
  auto it = _idToEvent.find(handle);
  if (it != _idToEvent.end())
  {
    GameEvent::Type type = it->second;
    auto& handlers = _handlersByWindow[type];
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
  auto it = _handlersByWindow.find(event._type);
  if (it == _handlersByWindow.end())
  {
    LOG_DEBUG("no handlers registered for event" << LogKeyValue("type", (int)event._type));
    return;
  }

  // Send event to all listeners
  for (auto& h : it->second)
  {
    h.second(event);
  }
}
