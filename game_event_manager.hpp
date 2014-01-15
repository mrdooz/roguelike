#pragma once
#include "utils.hpp"

namespace rogue
{
  class GameState;
  class Entity;
  struct SpellBase;
  struct LootItem;

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

    GameEvent(Type type);
    ~GameEvent();

    union
    {
      int _damage;
      int _healing;
      LootItem* _item;
    };

    SpellBase* _spell;

    Entity* _agent;
    Entity* _target;

    Type _type;
  private:
    DISALLOW_COPY_AND_ASSIGN(GameEvent);
  };

  class GameEventManager
  {
  public:

    typedef function<void(const GameEvent&)> fnEventHandler;

    GameEventManager();

    void SendEvent(const GameEvent& event);

    u32 RegisterHandler(GameEvent::Type event, const fnEventHandler& handler);
    void UnregisterHandler(u32 handle);

  private:
    typedef pair<u32, fnEventHandler> HandlerPair;
    map<GameEvent::Type, vector<HandlerPair> > _handlersByWindow;
    unordered_map<u32, GameEvent::Type> _idToEvent;
    u32 _nextId;
  };
}