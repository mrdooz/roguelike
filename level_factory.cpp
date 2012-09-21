#include "precompiled.hpp"
#include "level_factory.hpp"
#include "monster.hpp"
#include "utils.hpp"
#include <set>

using namespace std;

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

struct Crossover {
  Crossover() : start(INT_MAX), finish(INT_MIN) {}
  Crossover(bool horiz, int pos, int from, int to, int start, int finish) : horiz(horiz), pos(pos), from(from), to(to), start(start), finish(finish) {}
  bool horiz;
  int pos;  // row if horizontal, col if vertical
  int from, to;
  int start, finish;
};

Level *LevelFactory::createLevel(int width, int height, const sf::Texture &envTexture, const sf::Texture &charTexture) {
  Level *level = new Level(width, height, envTexture, charTexture);

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

  map<pair<int, int>, Crossover> crossovers;

  // Find all the crossovers
  for (int i = 1; i < width-1; ++i) {
    int j = 1;
    while (j < height && roomIds[j*width+i] == 0)
      ++j;
    if (j == height)
      continue;
    int prev = roomIds[j*width+i];
    for (int j = 2; j < height-1; ++j) {
      int cur = roomIds[j*width+i];
      if (cur != 0 && cur != prev) {
        int a = min(cur, prev);
        int b = max(cur, prev);
        auto &c = crossovers[make_pair(a,b)];
        c = Crossover(true, j-1, a, b, min(c.start, i), max(c.start, i));
        prev = cur;
      }
    }
  }

  for (int i = 1; i < height-1; ++i) {
    int j = 1;
    while (j < width && roomIds[i*width+j] == 0)
      ++j;
    if (j == width)
      continue;
    int prev = roomIds[i*width+j];
    for (int j = 2; j < width-1; ++j) {
      int cur = roomIds[i*width+j];
      if (cur != 0 && cur != prev) {
        int a = min(cur, prev);
        int b = max(cur, prev);
        auto &c = crossovers[make_pair(a,b)];
        c = Crossover(false, j-1, a, b, min(c.start, i), max(c.finish, i));
        prev = cur;
      }
    }
  }

  for (auto &kv: crossovers) {
    auto &crossover = kv.second;
    int door = (int)randf(crossover.start, crossover.finish);
    if (crossover.horiz) {
      level->get(crossover.pos, door)._type = TileType::kFloor;
    } else {
      level->get(door, crossover.pos)._type = TileType::kFloor;
    }
  }


/*
  for (int i = 0; i < 10; ++i) {
    Monster *monster = new Monster();
    int r = 1 + (rand() % (height-2));
    int c = 1 + (rand() % (width-2));
    monster->_pos = Pos(r, c);
    auto &sprite = monster->_sprite;
    sprite.setPosition((float)r*3*8, (float)c*3*8);
    sprite.setScale(3.0f, 3.0f);
    sprite.setColor(sf::Color(255,255,255));
    //sprite.setColor(sf::Color(0,0,0));
    sprite.setTextureRect(sf::IntRect((rand() % 10)*8, (rand() % 10)*8, 8, 8));
    sprite.setTexture(charTexture);
    level->_monsters.push_back(monster);
  }
*/

  FILE *f = fopen("d:\\temp\\level1.txt", "wt");
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      int id = roomIds[i*width+j];
      //fprintf(f, "%c", level->get(i,j)._type == TileType::kWall ? 'X' : ('A' + roomIds[i*width+j]));
      fprintf(f, "%c", level->get(i,j)._type == TileType::kWall ? 'X' : ' ');
      //fprintf(f, "%c", id == 0 ? 'X' : 'A' + id);// level->get(i,j)._type == TileType::kWall ? 'X' : ('A' + roomIds[i*width+j]));
    }
    fprintf(f, "\n");
  }
  fclose(f);

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
