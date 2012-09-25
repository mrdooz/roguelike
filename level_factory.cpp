#include "precompiled.hpp"
#include "level_factory.hpp"
#include "monster.hpp"
#include "utils.hpp"
#include <set>

using namespace std;

#define DEBUG_MAP 1

struct Room {
  Room(Room *parent) : parent(parent) {}

  sf::IntRect rect;
  vector<Room *> children;
  Room *parent;
};

const int cMaxDepth = 2;

Room *subdivide(Level *level, int *roomIds, int pitch, int &curRoom, int top, int left, int width, int height, int depth, Room *parent) {

  Room *cur = new Room(parent);
  cur->rect = sf::IntRect(left, top, width, height);

  bool horiz = !!(rand() % 2);

  if (horiz) {

    int split = (int)gaussianRand(height/2.0f, height/4.0f);
    for (int i = 0; i < width; ++i) {
      level->get(top+split, left+i)._type = TileType::kWall;
    }

    if (depth <= cMaxDepth && width > 5 && height > 5) {
      cur->children.push_back(subdivide(level, roomIds, pitch, curRoom, top, left, width, split+1, depth + 1, cur));
      cur->children.push_back(subdivide(level, roomIds, pitch, curRoom, top + split, left, width, height - split, depth + 1, cur));
    } else {
      for (int i = top+1; i < top + height-1; ++i) {
        for (int j = left+1; j < left + width-1; ++j) {
          roomIds[i*pitch+j] = i < top + split ? curRoom : i == top + split ? 0 : curRoom + 1;
        }
      }
      curRoom += 2;
    }

  } else {

    int split = (int)gaussianRand(width/2.0f, width/4.0f);
    for (int i = 0; i < height; ++i) {
      level->get(top+i, left+split)._type = TileType::kWall;
    }

    if (depth <= cMaxDepth && width > 5 && height > 5) {
      cur->children.push_back(subdivide(level, roomIds, pitch, curRoom, top, left, split+1, height, depth + 1, cur));
      cur->children.push_back(subdivide(level, roomIds, pitch, curRoom, top, left + split, width - split, height, depth + 1, cur));
    } else {
      for (int i = top+1; i < top + height-1; ++i) {
        for (int j = left+1; j < left + width-1; ++j) {
          roomIds[i*pitch+j] = j < left + split ? curRoom : j == left + split ? 0 : curRoom + 1;
        }
      }
      curRoom += 2;
    }
  }

  return cur;
}

struct Wall {
  Wall() : start(INT_MAX), finish(INT_MIN) {}
  Wall(bool horiz, int pos, int from, int to, int start, int finish) : horiz(horiz), pos(pos), from(from), to(to), start(start), finish(finish) {}
  bool horiz;
  int pos;  // row if horizontal, col if vertical
  int from, to;
  int start, finish;
};

Level *LevelFactory::createLevel(int width, int height) {
  Level *level = new Level(width, height);

  // fill floor
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      level->get(i, j)._type = TileType::kFloor;
    }
  }

  // outer wall
  for (int i = 0; i < width; ++i) {
    level->get(0, i)._type = TileType::kWall;
    level->get(height-1, i)._type = TileType::kWall;
  }

  for (int i = 0; i < height; ++i) {
    level->get(i, 0)._type = TileType::kWall;
    level->get(i, width-1)._type = TileType::kWall;
  }

  int roomCount = 1;
  vector<int> roomIds(width*height);
  Room *root = subdivide(level, roomIds.data(), width, roomCount, 0, 0, width, height, 0, nullptr);

  map<pair<int, int>, Wall> walls;

  // Find all the walls
  for (int i = 1; i < height-1; ++i) {
    for (int j = 1; j < width-1; ++j) {
      int x0 = roomIds[i*width+j-1];
      int x1 = roomIds[i*width+j];
      int x2 = roomIds[i*width+j+1];

      if (x0 != 0 && x1 == 0 && x2 != 0) {
        int a = min(x0, x2);
        int b = max(x0, x2);
        auto &c = walls[make_pair(a,b)];
        c = Wall(false, j, a, b, min(c.start, i), max(c.finish, i));

      } else {
        int y0 = roomIds[(i-1)*width+j];
        int y1 = roomIds[(i)*width+j];
        int y2 = roomIds[(i+1)*width+j];

        if (y0 != 0 && y1 == 0 && y2 != 0) {

          int a = min(y0, y2);
          int b = max(y0, y2);
          auto &c = walls[make_pair(a,b)];
          c = Wall(true, i, a, b, min(c.start, j), max(c.finish, j));
        }
      }
    }
  }

  for (auto &kv: walls) {
    auto &Wall = kv.second;
    int p = Wall.pos;
    while (true) {
      int door = (int)randf(Wall.start, Wall.finish);
      if (Wall.horiz) {
        if (level->get(p-1, door)._type != TileType::kWall && level->get(p+1, door)._type != TileType::kWall) {
          level->get(p, door)._type = TileType::kFloor;
          break;
        }
      } else {
        if (level->get(door, p-1)._type != TileType::kWall && level->get(door, p+1)._type != TileType::kWall) {
          level->get(door, p)._type = TileType::kFloor;
          break;
        }
      }
    }
  }

#if DEBUG_MAP

  vector<Pos> p;
  level->calcPath(Pos(1,1), Pos(level->_height-2, 2), &p);
  for (auto &pp : p) {
    roomIds[pp.y*width+pp.x] = -2;
  }

  FILE *f = fopen("d:\\temp\\level1.txt", "wt");
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      int id = roomIds[i*width+j];
      //fprintf(f, "%c", level->get(i,j)._type == TileType::kWall ? 'X' : ('a' + roomIds[i*width+j]));
      fprintf(f, "%c", id == -2 ? '.' : level->get(i,j)._type == TileType::kWall ? 'X' : ' ');
      //fprintf(f, "%c", id == 0 ? 'X' : 'A' + id);// level->get(i,j)._type == TileType::kWall ? 'X' : ('A' + roomIds[i*width+j]));
    }
    fprintf(f, "\n");
  }
  fclose(f);
#endif

  return level;
}

LevelFactory *LevelFactory::_instance;

bool LevelFactory::create() {
  assert(!_instance);
  _instance = new LevelFactory;
  return true;
}

void LevelFactory::close() {
  assert(_instance);
  delete exch_null(_instance);
}

LevelFactory &LevelFactory::instance() {
  assert(_instance);
  return *_instance;
}
