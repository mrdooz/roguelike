#include "precompiled.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "utils.hpp"

Level::Level(int width, int height, const sf::Texture &envTexture, const sf::Texture &charTexture)
  : _width(width)
  , _height(height)
  , _envTexture(envTexture)
  , _charTexture(charTexture)
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

void Level::draw(sf::RenderWindow *window, const Pos &topLeft, int rows, int cols) {

  // Recreate the sprites if the dimensions change
  if (_tileSprites.size() != rows * cols) {
    _tileSprites.resize(rows*cols);

    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        auto &sprite = _tileSprites[i*cols+j];
        sprite.setPosition((float)j*24, (float)i*24);
        sprite.setScale(3.0f, 3.0f);
        sprite.setTexture(_envTexture);
      }
    }

  }

  int idx = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      int r = topLeft.row + i;
      int c = topLeft.col + j;
      if (!inside(r, c))
        continue;
      Tile &tile = get(r, c);
      sf::Sprite &sprite = _tileSprites[idx++];

      if (tile._type == TileType::kWall) {
        if (inside(r+1, c) && get(r+1, c)._type != TileType::kWall) {
          sprite.setTextureRect(sf::IntRect((int)Tiles::wallH*8, 0, 8, 8));
        } else {
          sprite.setTextureRect(sf::IntRect((int)Tiles::wallV*8, 0, 8, 8));
        }
      } else if (tile._type == TileType::kFloor) {
        sprite.setTextureRect(sf::IntRect((int)Tiles::floorC*8, 0, 8, 8));

      } else {
        assert(false);
      }

      sprite.setColor(sf::Color(tile._visited, tile._visited, tile._visited));
      window->draw(sprite);
    }
  }


  for (auto &monster : _monsters)
    window->draw(monster->_sprite);
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

