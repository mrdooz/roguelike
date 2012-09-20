#include "precompiled.hpp"
#include "level_factory.hpp"
#include "monster.hpp"
#include "utils.hpp"

Level *LevelFactory::createLevel(int width, int height, const sf::Texture &envTexture, const sf::Texture &charTexture) {
  Level *level = new Level;

  level->_width = width;
  level->_height = height;

  level->_tiles.resize(width*height);

  for (int i = 1; i < height-1; ++i) {
    for (int j = 1; j < width-1; ++j) {
      auto &tile = level->get(i, j);
      tile._type = TileType::kFloor;
      auto &sprite = tile._sprite;
      sprite.setPosition((float)j*24, (float)i*24);
      sprite.setScale(3.0f, 3.0f);
      sprite.setColor(sf::Color(0,0,0,0));
      sprite.setTextureRect(sf::IntRect((int)Tiles::floorC*8, 0, 8, 8));
      sprite.setTexture(envTexture);
    }
  }

  for (int j = 0; j < 2; ++j) {
    for (int i = 0; i < width; ++i) {
      auto &tile = level->get(j*(height-1), i);
      tile._type = TileType::kWall;
      auto &sprite = tile._sprite;
      sprite.setPosition((float)i*3*8, (float)j*3*8*(height-1));
      sprite.setScale(3.0f, 3.0f);
      sprite.setColor(sf::Color(255,255,255));
      sprite.setTextureRect(sf::IntRect((int)Tiles::wallH*8, 0, 8, 8));
      sprite.setTexture(envTexture);
    }
  }

  for (int j = 0; j < 2; ++j) {
    for (int i = 0; i < height; ++i) {
      auto &tile = level->get(i, j*(width-1));
      tile._type = TileType::kWall;
      auto &sprite = tile._sprite;
      sprite.setPosition((float)j*3*8*(width-1), (float)i*3*8);
      sprite.setScale(3.0f, 3.0f);
      sprite.setColor(sf::Color(255,255,255));
      sprite.setTextureRect(sf::IntRect((int)Tiles::wallH*8, 0, 8, 8));
      sprite.setTexture(envTexture);
    }
  }

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
