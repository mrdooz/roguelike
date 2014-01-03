#include "level_factory.hpp"
#include "monster.hpp"
#include "utils.hpp"

using namespace rogue;

#ifdef _WIN32
#ifdef _DEBUG
#define DEBUG_MAP 1
#endif
#endif

struct Room
{
  Room(size_t parent) : parent(parent) {}

  sf::IntRect rect;
  vector<size_t> children;
  size_t parent;
};

//-----------------------------------------------------------------------------
struct LevelSubdivide
{
  static const int cMaxDepth = 3;

  LevelSubdivide(Level* level)
    : _level(level)
    , _pitch(level->Width())
    , _roomIds(level->Width()*level->Height())
  {
  }

  void Run()
  {
    int roomCount = 1;
    Subdivide(roomCount, 0, 0, _level->Width(), _level->Height(), 0, ~0);
  }

  size_t Subdivide(
      int &curRoom,
      int top,
      int left,
      int width,
      int height,
      int depth,
      size_t parent)
  {
    size_t curIdx = _rooms.size();
    // Note, please stop trying to be smart and grab a reference into the vector here! It will
    // resize in the recursive call!
    Room room(parent);
    room.rect = sf::IntRect(left, top, width, height);

    // Should the splitting wall be horizontal or vertical
    bool horiz = !!(rand() % 2);
    if (horiz)
    {
      // Create a split, with a mean position in the middle of the room
      // TODO: What is the extreme point of the gaussian distribution? should this be clamped?
      int splitPos = (int)gaussianRand(height/2.0f, height/4.0f);
      for (int i = 0; i < width; ++i)
        _level->Get(left+i, top+splitPos)._type = Tile::Type::Wall;

      if (depth <= cMaxDepth && width > 5 && height > 5)
      {
        auto lhs = Subdivide(curRoom, top, left, width, splitPos+1, depth + 1, curIdx);
        auto rhs = Subdivide(curRoom, top + splitPos, left, width, height - splitPos, depth + 1, curIdx);
        room.children.push_back(lhs);
        room.children.push_back(rhs);
      }
      else
      {
        // At bottom of recursion, set the room ids (either to cur, 0 for the wall, or cur+1 for under the split)
        for (int i = top+1; i < top + height-1; ++i)
        {
          for (int j = left+1; j < left + width-1; ++j)
          {
            _roomIds[i*_pitch+j] = i < top + splitPos ? curRoom : i == top + splitPos ? 0 : curRoom + 1;
          }
        }
        curRoom += 2;
      }
    }
    else
    {
      int split = (int)gaussianRand(width/2.0f, width/4.0f);
      for (int i = 0; i < height; ++i)
        _level->Get(left+split, top+i)._type = Tile::Type::Wall;

      if (depth <= cMaxDepth && width > 5 && height > 5)
      {
        auto lhs = Subdivide(curRoom, top, left, split+1, height, depth + 1, curIdx);
        auto rhs = Subdivide(curRoom, top, left + split, width - split, height, depth + 1, curIdx);
        room.children.push_back(lhs);
        room.children.push_back(rhs);
      }
      else
      {
        for (int i = top+1; i < top + height-1; ++i)
        {
          for (int j = left+1; j < left + width-1; ++j)
          {
            _roomIds[i*_pitch+j] = j < left + split ? curRoom : j == left + split ? 0 : curRoom + 1;
          }
        }
        curRoom += 2;
      }
    }
    _rooms.emplace_back(room);
    return curIdx;
  }

  Level* _level;
  size_t _pitch;
  vector<int> _roomIds;
  vector<Room> _rooms;
};

//-----------------------------------------------------------------------------
struct Wall
{
  enum class Orientation
  {
    Horizontal,
    Vertical,
  };

  Wall() : start(INT_MAX), finish(INT_MIN) {}
  Wall(Orientation orientation, int pos, int from, int to, int start, int finish)
    : orientation(orientation), pos(pos), from(from), to(to), start(start), finish(finish) {}
  Orientation orientation;
  int pos;            // Row if horizontal, col if vertical
  int from, to;       // The rooms the wall splits
  int start, finish;  // Extents of the wall
};

//-----------------------------------------------------------------------------
Level *LevelFactory::CreateLevel(int difficulty, int width, int height)
{
  Level* level = new Level(difficulty, width, height);

  // fill floor
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      level->Get(x,y)._type = Tile::Type::Floor;
    }
  }

  // outer walls
  for (int i = 0; i < width; ++i)
  {
    level->Get(i,0)._type = Tile::Type::Wall;
    level->Get(i,height-1)._type = Tile::Type::Wall;
  }

  for (int i = 0; i < height; ++i)
  {
    level->Get(0,i)._type = Tile::Type::Wall;
    level->Get(width-1,i)._type = Tile::Type::Wall;
  }

  LevelSubdivide sub(level);
  sub.Run();
  auto& roomIds = sub._roomIds;

  map<pair<int, int>, Wall> walls;

  // Find all the walls
  for (int i = 1; i < height-1; ++i)
  {
    for (int j = 1; j < width-1; ++j)
    {
      // Grab 3 adjacent horizontal tiles
      int x0 = roomIds[i*width+j-1];
      int x1 = roomIds[i*width+j+0];
      int x2 = roomIds[i*width+j+1];

      // Check if middle tile is a wall (and the outer tiles aren't)
      if (x0 != 0 && x1 == 0 && x2 != 0)
      {
        int a = min(x0, x2);
        int b = max(x0, x2);
        // Create an entry for a wall between room 'a' and room 'b', and update the extents of it.
        auto& wall = walls[make_pair(a,b)];
        wall = Wall(Wall::Orientation::Vertical, j, a, b, min(wall.start, i), max(wall.finish, i));
      }
      else
      {
        int y0 = roomIds[(i-1)*width+j];
        int y1 = roomIds[(i+0)*width+j];
        int y2 = roomIds[(i+1)*width+j];

        if (y0 != 0 && y1 == 0 && y2 != 0)
        {
          int a = min(y0, y2);
          int b = max(y0, y2);
          auto& wall = walls[make_pair(a,b)];
          wall = Wall(Wall::Orientation::Horizontal, i, a, b, min(wall.start, j), max(wall.finish, j));
        }
      }
    }
  }

  // Make doors in the walls
  for (auto &kv: walls)
  {
    auto& wall = kv.second;
    int p = wall.pos;
    while (true)
    {
      int door = (int)randf((float)wall.start, (float)wall.finish);
      if (wall.orientation == Wall::Orientation::Horizontal)
      {
        // Check that each side of the candidate door isn't a wall (avoid T junctions etc)
        if (level->Get(door, p-1)._type != Tile::Type::Wall && level->Get(door, p+1)._type != Tile::Type::Wall)
        {
          level->Get(door, p)._type = Tile::Type::Floor;
          break;
        }
      }
      else
      {
        // Vertical wall..
        if (level->Get(p-1, door)._type != Tile::Type::Wall && level->Get(p+1, door)._type != Tile::Type::Wall)
        {
          level->Get(p, door)._type = Tile::Type::Floor;
          break;
        }
      }
    }
  }

  DebugDump(level, roomIds, width, height);

  CreateMonsters(level);
  CreateDrops(level);
  level->Init();

  return level;
}

//-----------------------------------------------------------------------------
void LevelFactory::DebugDump(Level* level, vector<int>& roomIds, int width, int height)
{
#if DEBUG_MAP
  vector<Pos> p;
  level->calcPath(Pos(1,1), Pos(2, level->_height-2), &p);
  for (auto &pp : p) {
    roomIds[pp.y*width+pp.x] = -2;
  }

  FILE *f = fopen("d:\\temp\\level1.txt", "wt");
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      int id = roomIds[i*width+j];
      //fprintf(f, "%c", level->get(i,j)._type == Tile::Type::Wall ? 'X' : ('a' + roomIds[i*width+j]));
      fprintf(f, "%c", id == -2 ? '.' : level->Get(i,j)._type == Tile::Type::Wall ? 'X' : ' ');
      //fprintf(f, "%c", id == 0 ? 'X' : 'A' + id);// level->get(i,j)._type == Tile::Type::Wall ? 'X' : ('A' + roomIds[i*width+j]));
    }
    fprintf(f, "\n");
  }
  fclose(f);
#endif
}

//-----------------------------------------------------------------------------
static bool FindEmptyTile(Level* level, Pos& pos)
{
  int w = level->Width();
  int h = level->Height();

  int numAttemps = 100;
  while (numAttemps--)
  {
    int x = 1 + (rand() % (w-2));
    int y = 1 + (rand() % (h-2));
    auto &tile = level->Get(x, y);
    if (tile.ValidMoveLocation())
    {
      pos = Pos(x,y);
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
void LevelFactory::CreateDrops(Level* level)
{
  int numDrops = 20;
  while (numDrops--)
  {
    Pos pos;
    if (FindEmptyTile(level, pos))
    {
      auto& tile = level->Get(pos);
      LootItem item(level->GenerateLoot(level->_difficulty));
      tile._items.push_back(item);
    }

  }
}

//-----------------------------------------------------------------------------
void LevelFactory::CreateMonsters(Level* level)
{
  for (int i = 0; i < 10; ++i)
  {
    Pos pos;
    if (FindEmptyTile(level, pos))
    {
      Monster *monster = new Monster();
      auto &tile = level->Get(pos);
      tile._monster = monster;
      monster->SetPos(pos);
      monster->_curHealth = 10;
      monster->_maxHealth = 10;
      monster->_visited.resize(level->Width() * level->Height(), 0);
      monster->_visited[level->PosToIndex(pos)] = 1;

      auto &sprite = monster->_sprite;
      sprite.setScale(3.0f, 3.0f);
      sprite.setColor(sf::Color(255,255,255));
      monster->_monsterType = (MonsterType)(rand() % (int)MonsterType::cNumMonsters);
      level->_monsters.push_back(monster);
    }

  }
}
