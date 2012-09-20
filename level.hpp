#ifndef _LEVEL_HPP_
#define _LEVEL_HPP_

#include "game.hpp"

struct Monster;

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
  Tile() : _visited(0), _containsPlayer(false) {}
  TileType _type;
  bool _containsPlayer;
  sf::Sprite _sprite;
  int _visited;
};

class Level {
  friend class LevelFactory;
public:
  ~Level();

  Tile &get(int row, int col) { return _tiles[row*_width+col]; }
  Tile &get(const Pos &pos) { return _tiles[pos.row*_width+pos.col]; }

  void draw(sf::RenderWindow *window);
  bool movable(const Pos &pos);
  bool validPosition(const Pos &pos);
  void visitTile(const Pos &pos);

private:
  std::vector<Tile> _tiles;
  std::vector<Monster *> _monsters;
  int _width;
  int _height;
};


#endif
