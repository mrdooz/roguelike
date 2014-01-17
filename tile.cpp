#include "tile.hpp"
#include "player.hpp"
#include "monster.hpp"

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

//-----------------------------------------------------------------------------
bool Tile::ValidMoveLocation() const
{
  return !_player && !_monster && _type != Type::Wall;

}
