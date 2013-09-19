#ifndef _LEVEL_HPP_
#define _LEVEL_HPP_

#include "types.hpp"

namespace rogue
{
  class Monster;
  class Player;

  enum class TileType {
    kFloor,
    kWall,
    kStairUp,
    kStairDown,
  };

  struct Tile {
    Tile() : _visited(0), _player(nullptr), _monster(nullptr) {}
    TileType _type;
    Player *_player;
    Monster *_monster;
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
    void moveMonster(Monster *m, const Pos &oldPos, const Pos &newPos);

    void initPlayer(Player *p, const Pos &pos);

    void initMonsters();

    bool inside(int row, int col) const;
    bool inside(const Pos &pos) const { return inside(pos.row, pos.col); }
    bool tileFree(const Pos &pos) const;

    Tile &get(int row, int col);
    Tile &get(const Pos &pos);
    const Tile &get(const Pos &pos) const;

    vector<Monster *> &monsters() { return _monsters; }

    bool calcPath(const Pos &start, const Pos &end, vector<Pos> *path);

    void monsterKilled(Monster *m);

    Player *playerAt(const Pos &pos);
    Monster *monsterAt(const Pos &pos);

  private:

    void updateFog(const Pos &pos);

    vector<sf::Sprite> _tileSprites;
    vector<Tile> _tiles;
    vector<Monster *> _monsters;
    int _width;
    int _height;
  };

}

#endif
