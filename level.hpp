#ifndef _LEVEL_HPP_
#define _LEVEL_HPP_

#include "types.hpp"

class Monster;
class Player;


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
  friend class Renderer;
public:
  Level(int width, int height);
  ~Level();

  bool validDestination(const Pos &pos);
  void movePlayer(Player *p, const Pos &oldPos, const Pos &newPos);
  void initPlayer(Player *p, const Pos &pos);

  bool inside(int row, int col) const;
  bool inside(const Pos &pos) const { return inside(pos.row, pos.col); }

  Tile &get(int row, int col);
  Tile &get(const Pos &pos);

private:

  void updateFog(const Pos &pos);

  std::vector<sf::Sprite> _tileSprites;
  std::vector<Tile> _tiles;
  std::vector<Monster *> _monsters;
  int _width;
  int _height;
};

#endif
