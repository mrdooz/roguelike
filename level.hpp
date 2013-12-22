#ifndef _LEVEL_HPP_
#define _LEVEL_HPP_

#include "types.hpp"
#include "roguelike.hpp"

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

  struct Tile
  {
    Tile() : _player(nullptr), _monster(nullptr), _visited(0), _selected(false) {}
    bool IsEmpty() const { return !_player && !_monster; }
    TileType _type;
    Player *_player;
    Monster *_monster;
    int _visited;
    bool _selected;
  };

  class Level
  {
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

    bool Inside(int row, int col) const;
    bool Inside(const Pos &pos) const { return Inside(pos.row, pos.col); }
    bool tileFree(const Pos &pos) const;

    int Width() const { return _width; }
    int Height() const { return _height; }

    Tile &Get(int row, int col);
    Tile &Get(const Pos &pos);
    const Tile &Get(const Pos &pos) const;

    const vector<Monster* >& monsters() { return _monsters; }

    bool calcPath(const Pos &start, const Pos &end, vector<Pos> *path);

    void monsterKilled(Monster *m);

    Player *playerAt(const Pos &pos);
    Monster *monsterAt(const Pos &pos);

  private:

    Pos IndexToPos(size_t idx) const;
    size_t PosToIndex(const Pos& pos) const;
    void updateFog(const Pos &pos);

    vector<Tile> _tiles;
    vector<Monster* > _monsters;
    int _width;
    int _height;
  };

}

#endif
