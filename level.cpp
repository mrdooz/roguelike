#include "precompiled.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "utils.hpp"

using namespace rogue;

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

bool Level::calcPath(const Pos &start, const Pos &end, vector<Pos> *path)
{

  struct PosLink
  {
    PosLink(size_t idx, size_t linkIdx, size_t parentIdx) : idx(idx), linkIdx(linkIdx), parentIdx(parentIdx) {}
    size_t idx;
    // note, these indices push into the links vector
    size_t linkIdx;
    size_t parentIdx;
  };

  vector<PosLink> links;

  struct Distance
  {
    Distance(const Level& level, const Pos &dst) : level(level), dst(dst) {}
    bool operator()(const PosLink& a, const PosLink& b)
    {
      const Pos& p0 = level.IndexToPos(a.idx);
      const Pos& p1 = level.IndexToPos(b.idx);

      float ax = fabsf((float)(p0.x - dst.x));
      float ay = fabsf((float)(p0.y - dst.y));

      float bx = fabsf((float)(p1.x - dst.x));
      float by = fabsf((float)(p1.y - dst.y));
      return ax + ay > bx + by;
    }
    const Level& level;
    Pos dst;
  };

  auto q = priority_queue<PosLink, vector<PosLink>, Distance>(Distance(*this, end));
  PosLink s(PosToIndex(start), 0, ~0);
  links.push_back(s);
  q.push(s);
  vector<u32> visited(_tiles.size() / 32 + 1);
  auto IsVisited = [&](const Pos& p)
  {
    size_t idx = PosToIndex(p);
    return (visited[idx/32] & (1 << (idx & 0x1f))) > 0;
  };

  auto SetVisited = [&](const Pos& p)
  {
    size_t idx = PosToIndex(p);
    visited[idx/32] |= (1 << (idx & 0x1f));
  };

  while (!q.empty())
  {
    auto cur = q.top();
    q.pop();

    if (cur.idx == PosToIndex(end))
    {
      // If we've reached the end, trace backwards to find the path
      vector<Pos> tmp;
      while (cur.parentIdx != ~0)
      {
        tmp.push_back(IndexToPos(cur.idx));
        cur = links[cur.parentIdx];
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
      Pos cand(IndexToPos(cur.idx) + ofs);
      if (inside(cand) && get(cand)._type != TileType::kWall && !IsVisited(cand))
      {
        PosLink x(PosToIndex(cand), links.size(), cur.linkIdx);
        links.push_back(x);
        q.push(x);
        SetVisited(cand);
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

Player *Level::playerAt(const Pos &pos)
{
  return inside(pos) ? get(pos)._player : nullptr;
}

Monster *Level::monsterAt(const Pos &pos)
{
  return inside(pos) ? get(pos)._monster : nullptr;
}

Pos Level::IndexToPos(size_t idx) const
{
  if (_width == 0)
    return Pos();

  return Pos(idx/_width, idx % _width);
}

size_t Level::PosToIndex(const Pos& pos) const
{
  return pos.row * _width + pos.col;
}
