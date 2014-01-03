#include "tile.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
Tile::Tile()
  : _player(nullptr)
  , _monster(nullptr)
  , _visited(0)
  , _selected(false)
{
}

//-----------------------------------------------------------------------------
bool Tile::IsEmpty(bool ignoreItems) const
{
  return !_player && !_monster && (ignoreItems || _items.empty());
}

