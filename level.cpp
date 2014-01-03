#include "level.hpp"
#include "monster.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "player.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
Level::Level(int difficulty, int width, int height)
  : _difficulty(difficulty)
  , _width(width)
  , _height(height)
{
  _tiles.resize(width*height);
}

//-----------------------------------------------------------------------------
Level::~Level()
{
  seq_delete(&_monsters);
}

//-----------------------------------------------------------------------------
void Level::Init()
{
  GAME_EVENT->RegisterHandler(GameEvent::Type::Death, bind(&Level::OnDeath, this, _1));
}

//-----------------------------------------------------------------------------
LootItem Level::GenerateLoot(int level)
{
  // Spawn loot!
  int rnd = rand() % 20;
  if (rnd < 10)
  {
    LootItemEx obj(LootItem::Type::Gold);
    obj._numGold = (10 + (rand() % level) ) * 100;
    return obj;
  }
  else if (rnd < 12)
  {
    LootItemEx obj(LootItem::Type::HealthPotion);
    obj._numPots = 1;
    return obj;
  }
  else if (rnd < 15)
  {
    LootItemEx obj(LootItem::Type::ManaPotion);
    obj._numPots = 1;
    return obj;
  }
  else if (rnd < 17)
  {
    LootItemEx obj(LootItem::Type::ArmorUpgrade);
    obj._numUpgrades = 1;
    return obj;
  }
  else
  {
    LootItemEx obj(LootItem::Type::WeaponUpgrade);
    obj._numUpgrades = 1;
    return obj;
  }
}

//-----------------------------------------------------------------------------
void Level::OnDeath(const GameEvent& event)
{
  Entity* target = event._target;
  if (target->GetType() == Entity::Type::Player)
  {

  }
  else
  {
    // Spawn loot!
    LootItem item(GenerateLoot(target->Level()));
    Get(target->GetPos())._items.push_back(item);
    MonsterKilled(static_cast<Monster*>(event._target));
  }
}

//-----------------------------------------------------------------------------
void Level::MonsterKilled(Monster* m)
{
  auto it = find(_monsters.begin(), _monsters.end(), m);
  if (it == _monsters.end())
    return;

  // Clean up structues referencing the monster
  Get(m->GetPos())._monster = nullptr;
  _monsters.erase(it);
  delete m;
}

//-----------------------------------------------------------------------------
bool Level::Inside(int x, int y) const
{
  return x >= 0 && x < _width && y >= 0 && y < _height;
}

Tile &Level::Get(int x, int y)
{
  return _tiles[y*_width+x];
}

Tile &Level::Get(const Pos &pos)
{
  return _tiles[pos.y*_width+pos.x];
}

const Tile &Level::Get(const Pos &pos) const
{
  return _tiles[pos.y*_width+pos.x];
}

//-----------------------------------------------------------------------------
bool Level::ValidDestination(const Pos &pos)
{
  if (!Inside(pos))
    return false;
  auto &tile = Get(pos);
  return tile._type != Tile::Type::kWall && tile.IsEmpty(true);
}

//-----------------------------------------------------------------------------
void Level::initPlayer(Player *p, const Pos &pos) {

  assert(Inside(pos));

  Tile &newTile = Get(pos);
  assert(!newTile._player);
  newTile._player = p;

  updateFog(pos);
}

//-----------------------------------------------------------------------------
void Level::MoveMonster(Monster *m, const Pos &newPos)
{
  Pos oldPos(m->GetPos());

  assert(Inside(oldPos) && Inside(newPos));

  Tile &oldTile = Get(oldPos);
  assert(oldTile._monster == m);
  oldTile._monster = nullptr;

  Tile &newTile = Get(newPos);
  assert(!newTile._monster);
  newTile._monster = m;
}

//-----------------------------------------------------------------------------
void Level::MovePlayer(Player* p, const Pos& newPos)
{
  Pos oldPos(p->GetPos());
  assert(Inside(oldPos) && Inside(newPos));

  Tile& oldTile = Get(oldPos);
  assert(oldTile._player == p);
  oldTile._player = nullptr;

  Tile& newTile = Get(newPos);
  assert(!newTile._player);
  newTile._player = p;

  updateFog(newPos);
}

void Level::updateFog(const Pos &pos)
{
  int lightRadius = 2;
  for (int i = -lightRadius; i <= lightRadius; ++i) {
    for (int j = -lightRadius; j <= lightRadius; ++j) {
      Pos p(pos.x + j, pos.y + i);
      if (!Inside(p))
        continue;
      int dy = i < 0 ? -i : i;
      int dx = j < 0 ? -j : j;
      auto &tile = Get(p);
      int v = std::min(2, std::max(0, 2 - (int)(sqrt(dx*dx+dy*dy) + 0.5f)));
      tile._visited = std::min(255, tile._visited + 64 * v);
    }
  }
}

bool Level::tileFree(const Pos &pos) const
{
  auto &tile = Get(pos);
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
      if (Inside(cand) && Get(cand)._type != Tile::Type::kWall && !IsVisited(cand))
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


Player *Level::playerAt(const Pos &pos)
{
  return Inside(pos) ? Get(pos)._player : nullptr;
}

Monster *Level::monsterAt(const Pos &pos)
{
  return Inside(pos) ? Get(pos)._monster : nullptr;
}

Pos Level::IndexToPos(size_t idx) const
{
  if (_width == 0)
    return Pos();

  return Pos(idx % _width, idx/_width);
}

size_t Level::PosToIndex(const Pos& pos) const
{
  return pos.y * _width + pos.x;
}

//-----------------------------------------------------------------------------
bool Level::IsVisible(const Pos& a, const Pos& b)
{
  // Step between 'a' and 'b', and return false is there is a wall between
  int dx = b.x - a.x;
  int dy = b.y - a.y;

  if (dx == 0 && dy == 0)
    return true;

  int steps = IntAbs(dx) + IntAbs(dy);

  int stepx = (dx << 16) / steps;
  int stepy = (dx << 16) / steps;

  int curx = a.x << 16;
  int cury = a.y << 16;

  for (int i = 0; i < steps; ++i)
  {
    curx += stepx;
    cury += stepy;

    auto& tile = Get(curx >> 16, cury >> 16);
    if (tile._type == Tile::Type::kWall)
      return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
Pos Level::StepTowards(const Pos& a, const Pos& b)
{
  Pos ofs[4] = { Pos(-1, 0), Pos(1, 0), Pos(0, -1), Pos(0, 1) };
  Pos best(a);
  float dst = Dist(b, a);

  for (const auto& p : ofs)
  {
    Pos cand = a + p;
    if (Get(cand).IsEmpty(true))
    {
      float tmp = Dist(cand, b);
      if (tmp < dst)
      {
        best = cand;
        dst = tmp;
      }
    }
  }

  return best;
}
