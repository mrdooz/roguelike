#include "precompiled.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "utils.hpp"

Level::Level(int width, int height)
  : _width(width)
  , _height(height)
{
  _tiles.resize(width*height);
}

Level::~Level() {
  seq_delete(&_monsters);
}

bool Level::inside(int row, int col) const {
  return row >= 0 && row < _height && col >= 0 && col < _width;
}

Tile &Level::get(int row, int col) { 
  return _tiles[row*_width+col]; 
}

Tile &Level::get(const Pos &pos) { 
  return _tiles[pos.row*_width+pos.col]; 
}

bool Level::validDestination(const Pos &pos) {
  if (!inside(pos))
    return false;
  auto &tile = get(pos);
  return tile._type != TileType::kWall && !tile._player;

}

void Level::initPlayer(Player *p, const Pos &pos) {

  assert(inside(pos));

  Tile &newTile = get(pos);
  assert(!newTile._player);
  newTile._player = p;

  updateFog(pos);
}

void Level::movePlayer(Player *p, const Pos &oldPos, const Pos &newPos) {

  assert(inside(oldPos) && inside(newPos));

  Tile &oldTile = get(oldPos);
  assert(oldTile._player == p);
  oldTile._player = nullptr;

  Tile &newTile = get(newPos);
  assert(!newTile._player);
  newTile._player = p;

  updateFog(newPos);
}

void Level::updateFog(const Pos &pos) {
  for (int i = -1; i <= 1; ++i) {
    for (int j = -1; j <= 1; ++j) {
      Pos p(pos.row + i, pos.col + j);
      if (!inside(p))
        continue;
      int dy = i < 0 ? -i : i;
      int dx = j < 0 ? -j : j;
      auto &tile = get(p);
      int v = std::min(2, std::max(0, 2 - (int)(sqrt(dx*dx+dy*dy) + 0.5f)));
      tile._visited = std::min(255, tile._visited + 64 * v);
    }
  }
}

