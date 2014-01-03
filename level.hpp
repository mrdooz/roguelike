#pragma once

#include "game_event_manager.hpp"
#include "loot_item.hpp"

namespace rogue
{
  class Monster;
  class Player;

  enum class TileType
  {
    kFloor,
    kWall,
    kStairUp,
    kStairDown,
  };

  struct Tile
  {
    Tile();
    bool IsEmpty(bool ignoreItems) const;
    
    deque<LootItem> _items;
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
    friend class GamePlayer;
  public:
    Level(int difficulty, int width, int height);
    ~Level();

    void Init();

    bool ValidDestination(const Pos &pos);
    void MovePlayer(Player *p, const Pos &oldPos, const Pos &newPos);

    void moveMonster(Monster *m, const Pos &oldPos, const Pos &newPos);

    void initPlayer(Player *p, const Pos &pos);

    bool Inside(int x, int y) const;
    bool Inside(const Pos &pos) const { return Inside(pos.x, pos.y); }
    bool tileFree(const Pos &pos) const;

    int Width() const { return _width; }
    int Height() const { return _height; }

    Tile &Get(int x, int y);
    Tile& Get(const Pos &pos);
    const Tile& Get(const Pos &pos) const;

    const vector<Monster* >& monsters() { return _monsters; }

    bool calcPath(const Pos &start, const Pos &end, vector<Pos> *path);


    Player *playerAt(const Pos &pos);
    Monster *monsterAt(const Pos &pos);

    LootItem GenerateLoot(int level);

  private:
    void OnDeath(const GameEvent& event);
    void MonsterKilled(Monster *m);

    Pos IndexToPos(size_t idx) const;
    size_t PosToIndex(const Pos& pos) const;
    void updateFog(const Pos &pos);

    vector<Tile> _tiles;
    vector<Monster* > _monsters;
    int _difficulty;
    int _width;
    int _height;
  };

}
