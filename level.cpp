#include "precompiled.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "utils.hpp"

Level::~Level() {
  seq_delete(&_monsters);
}

void Level::draw(sf::RenderWindow *window) {
  for (auto &tile: _tiles)
    window->draw(tile._sprite);

  for (auto &monster : _monsters)
    window->draw(monster->_sprite);
}

bool Level::movable(const Pos &pos) {
  auto &tile = get(pos);
  return tile._type != TileType::kWall && !tile._containsPlayer;
}

bool Level::validPosition(const Pos &pos) {
  bool inside = pos.row >= 0 && pos.row < _height && pos.col >= 0 && pos.col < _width;
  return inside && movable(pos);
}

void Level::visitTile(const Pos &pos) {

  get(pos)._containsPlayer = true;

  for (int i = -1; i <= 1; ++i) {
    for (int j = -1; j <= 1; ++j) {
      int r = pos.row + i;
      int c = pos.col + j;
      if (!validPosition(Pos(r,c)))
        continue;
      int dy = i < 0 ? -i : i;
      int dx = j < 0 ? -j : j;
      auto &tile = get(r, c);
      int v = std::min(2, std::max(0, 2 - (int)(sqrt(dx*dx+dy*dy) + 0.5f)));
      tile._visited += 64 * v;
      tile._visited = std::min(255, tile._visited);
      tile._sprite.setColor(sf::Color(tile._visited, tile._visited, tile._visited));
    }
  }

}
