#pragma once
#include "loot_item.hpp"

namespace rogue
{
  class GameState;
  class Entity;

  struct GameEvent
  {
    enum class Type
    {
      Attack,
      Heal,
      Death,
      LevelGained,
      ItemGained,
    };

    GameEvent(Type type) : _type(type) {}

    union
    {
      int _damage;
      int _healing;
      LootItem _item;
    };

    Entity* _agent;
    Entity* _target;

    Type _type;
  };

  class GameEventManager
  {
  public:

    typedef function<void(const GameEvent&)> fnEventHandler;

    GameEventManager();

    void SendEvent(const GameEvent& event);

    size_t RegisterHandler(GameEvent::Type event, const fnEventHandler& handler);
    void UnregisterHandler(size_t handle);

  private:
    typedef pair<size_t, fnEventHandler> HandlerPair;
    map<GameEvent::Type, vector<HandlerPair> > m_handlers;
    unordered_map<size_t, GameEvent::Type> m_IdToEvent;
    size_t m_nextId;
  };
}