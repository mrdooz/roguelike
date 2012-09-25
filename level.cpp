#include "precompiled.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "utils.hpp"
#include <queue>
#include <set>
#include <memory>

using namespace std;

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

const Tile &Level::get(const Pos &pos) const {
  return _tiles[pos.row*_width+pos.col];
}

bool Level::validDestination(const Pos &pos) {
  if (!inside(pos))
    return false;
  auto &tile = get(pos);
  return tile._type != TileType::kWall && !tile._player && !tile._monster;

}

void Level::initPlayer(Player *p, const Pos &pos) {

  assert(inside(pos));

  Tile &newTile = get(pos);
  assert(!newTile._player);
  newTile._player = p;

  updateFog(pos);
}

void Level::initMonsters() {
  for (int i = 0; i < 100; ++i) {
    Monster *monster = new Monster();
    int r, c;
    while (true) {
      r = 1 + (rand() % (_height-2));
      c = 1 + (rand() % (_width-2));
      auto &tile = get(r, c);
      if (tile._type == TileType::kFloor && !tile._monster && !tile._player) {
        tile._monster = monster;
        break;
      }
    }
    monster->_pos = Pos(r, c);
    auto &sprite = monster->_sprite;
    sprite.setScale(3.0f, 3.0f);
    sprite.setColor(sf::Color(255,255,255));
    monster->_type = (MonsterType)(rand() % (int)MonsterType::cNumMonsters);
    _monsters.push_back(monster);
  }
}

void Level::moveMonster(Monster *m, const Pos &oldPos, const Pos &newPos) {

  assert(inside(oldPos) && inside(newPos));

  Tile &oldTile = get(oldPos);
  assert(oldTile._monster == m);
  oldTile._monster = nullptr;

  Tile &newTile = get(newPos);
  assert(!newTile._monster);
  newTile._monster = m;
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

bool Level::tileFree(const Pos &pos) const {
  auto &tile = get(pos);
  return tile._monster == nullptr && tile._player == nullptr;
}

bool Level::calcPath(const Pos &start, const Pos &end, std::vector<Pos> *path) {

  struct PosLink {
    PosLink(const Pos &pos, shared_ptr<PosLink> parent) : pos(pos), parent(parent) {}
    Pos pos;
    shared_ptr<PosLink> parent;
  };

  struct Distance {
    Distance(const Pos &dst) : dst(dst) {}
    bool operator()(const shared_ptr<PosLink> &a, const shared_ptr<PosLink> &b) {
      float ax = (float)(a->pos.x - dst.x);
      float ay = (float)(a->pos.y - dst.y);

      float bx = (float)(b->pos.x - dst.x);
      float by = (float)(b->pos.y - dst.y);
      return sqrtf(ax*ax + ay*ay) > sqrtf(bx*bx+by*by);
    }
    Pos dst;
  };

  set<Pos> visited;
  auto q = priority_queue<PosLink *, vector<shared_ptr<PosLink>>, Distance>(Distance(end));
  q.push(shared_ptr<PosLink>(new PosLink(start, nullptr)));

  visited.insert(start);

  while (!q.empty()) {
    auto cur = q.top();
    q.pop();

    if (cur->pos == end) {
      vector<Pos> tmp;
      shared_ptr<PosLink> node = cur;
      while (node) {
        tmp.push_back(node->pos);
        node = node->parent;
      }
      path->resize(tmp.size());
      size_t cnt = tmp.size();
      for (size_t i = 0; i < cnt; ++i) {
        (*path)[cnt-1-i] = tmp[i];
      }
      return true;
    }

    Pos offsets[] = { Pos(1,0), Pos(-1,0), Pos(0, 1), Pos(0, -1) };
    for (auto &ofs : offsets) {
      Pos cand(cur->pos + ofs);
      if (inside(cand) && get(cand)._type != TileType::kWall && !contains(visited, cand)) {
        q.push(shared_ptr<PosLink>(new PosLink(cand, cur)));
        visited.insert(cand);
      }
    }

  }

  return false;
}

void Level::monsterKilled(Monster *m) {
  for (auto it = begin(_monsters); it != end(_monsters); ) {
    if (*it == m) {
      get(m->_pos)._monster = nullptr;
      _monsters.erase(it);
      delete m;
      return;
    } else {
      ++it;
    }
  }
}
