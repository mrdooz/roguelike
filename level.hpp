#ifndef _LEVEL_HPP_
#define _LEVEL_HPP_

#include "game.hpp"

struct Monster;
struct Player;

enum class Tiles {
  wallH,
  wallH_torch_anim_1,
  wAllH_torch_anim_2,
  wallH_crumbled,
  wallV,
  floorA,
  floorB,
  stairsUp,
  stairsDown,
  hole,
  trapdoorClosed,
  trapdoorOpen,
  floorC,
  floorRug,
  cNumTiles,
};

enum class TileType {
  kFloor,
  kWall,
  kStairUp,
  kStairDown,
};

struct Tile {
  Tile() : _visited(0), _player(nullptr) {}
  TileType _type;
  Player *_player;
  int _visited;
};

class Level {
  friend class LevelFactory;
public:
  Level(int width, int height, const sf::Texture &envTexture, const sf::Texture &charTexture);
  ~Level();

  void draw(sf::RenderWindow *window, const Pos &topLeft, int rows, int cols);
  bool validDestination(const Pos &pos);
  void movePlayer(Player *p, const Pos &oldPos, const Pos &newPos);
  void initPlayer(Player *p, const Pos &pos);

  bool inside(int row, int col) const;
  bool inside(const Pos &pos) const { return inside(pos.row, pos.col); }

  Tile &get(int row, int col);
  Tile &get(const Pos &pos);
  sf::Sprite &getEnvSprite(int row, int col);

private:

  void updateFog(const Pos &pos);

  std::vector<sf::Sprite> _tileSprites;
  std::vector<Tile> _tiles;
  std::vector<Monster *> _monsters;
  int _width;
  int _height;

  sf::Texture _envTexture;
  sf::Texture _charTexture;
};


#endif
