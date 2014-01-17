#pragma once
#include "loot_item.hpp"

namespace rogue
{
  class Monster;
  class Player;

  struct Tile
  {
    enum class Type
    {
      Floor,
      Wall,
      StairUp,
      StairDown,
    };

    Tile();
    bool IsEmpty(bool ignoreItems) const;
    bool ValidMoveLocation() const;

    deque<LootItem> _items;
    Tile::Type _type;
    intrusive_ptr<Player> _player;
    intrusive_ptr<Monster> _monster;
    int _visited;
    bool _selected;
  };
}
