#include "precompiled.hpp"
#include "level_factory.hpp"
#include "monster.hpp"
#include "utils.hpp"

using namespace std;

struct Room {
  Room(Room *parent) : parent(parent) {}

  sf::IntRect rect;
  vector<Room *> children;
  Room *parent;
};

Room *subdivide(Level *level, int top, int left, int width, int height, int depth, Room *parent) {

  if (depth > 3 || width <= 5 || height <= 5)
    return nullptr;

  Room *cur = new Room(parent);
  cur->rect = sf::IntRect(left, top, width, height);

  bool horiz = !!(rand() % 2);

  if (horiz) {

    int split = (int)gaussianRand(height/2.0f, height/4.0f);
    for (int i = 1; i < width; ++i) {
      level->get(top+split, left+i)._type = TileType::kWall;
    }

    if (Room *c = subdivide(level, top, left, width, split, depth + 1, cur))
      cur->children.push_back(c);

    if (Room *c = subdivide(level, top + split, left, width, height - split, depth + 1, cur))
      cur->children.push_back(c);

  } else {

    int split = (int)gaussianRand(width/2.0f, width/4.0f);
    for (int i = 1; i < height; ++i) {
      level->get(top+i, left+split)._type = TileType::kWall;
    }

    if (Room *c = subdivide(level, top, left, split, height, depth + 1, cur))
      cur->children.push_back(c);

    if (Room *c = subdivide(level, top, left + split, width - split, height, depth + 1, cur))
      cur->children.push_back(c);
  }

  return cur;
}

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

  Room *root = subdivide(level, 0, 0, width, height, 0, nullptr);

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
      fprintf(f, "%c", level->get(i,j)._type == TileType::kWall ? 'X' : ' ');
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
