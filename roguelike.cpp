#include "stdafx.h"
#include "utils.hpp"

#include <vector>
#include <assert.h>

using namespace std;

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

enum class PlayerClass : uint8 {
  kWizard,
  kRogue,
  kWarrior,
  kCleric,
};

enum class PlayerMode : uint8 {
  kExplore,
  kGuard,
  kFollow,
  kInteractive,
};

string playerClassToString(PlayerClass pc) {
  switch (pc) {
    case PlayerClass::kWizard: return "Wizard";
    case PlayerClass::kRogue: return "Rogue";
    case PlayerClass::kWarrior: return "Warrior";
    case PlayerClass::kCleric: return "Cleric";
    default: assert(false); return "";
  }
}

string playerModeToString(PlayerMode pm) {
  switch (pm) {
    case PlayerMode::kExplore: return "Explore";
    case PlayerMode::kGuard: return "Guard";
    case PlayerMode::kFollow: return "Follow";
    case PlayerMode::kInteractive: return "Interactive";
    default: assert(false); return "";
  }
}

struct Pos {
  Pos() : row(~0), col(~0) {}
  Pos(int r, int c) : row(r), col(c) {}
  friend Pos operator+(const Pos &a, const Pos &b) { return Pos(a.row + b.row, a.col + b.col); }
  friend Pos operator-(const Pos &a, const Pos &b) { return Pos(a.row - b.row, a.col - b.col); }
  int row, col;
};

struct Player {
  Player() : _mode(PlayerMode::kInteractive), _hasMoved(false), _activePlayer(false) {}

  string _name;

  int _intelligence;
  int _strength;
  int _dexterity;
  int _vitality;
  int _armor;

  int _health;
  int _mana;
  Pos _pos;

  bool _activePlayer;
  bool _hasMoved;

  sf::Sprite _sprite;
  PlayerMode _mode;
  PlayerClass _class;
};

struct Monster {
  sf::Sprite _sprite;
  Pos _pos;
};

struct Party {
  Party() : _curPlayer(0) {}
  int _curPlayer;
  vector<Player *> _players;
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
  Tile &get(int row, int col) { return _tiles[row*_width+col]; }
  Tile &get(const Pos &pos) { return _tiles[pos.row*_width+pos.col]; }

  void draw(sf::RenderWindow *window) {
    for (auto &tile: _tiles)
      window->draw(tile._sprite);

    for (auto &monster : _monsters)
      window->draw(monster._sprite);
  }

  bool movable(const Pos &pos) {
    auto &tile = get(pos);
    return tile._type != TileType::kWall && !tile._containsPlayer;
  }

  bool validPosition(const Pos &pos) {
    bool inside = pos.row >= 0 && pos.row < _height && pos.col >= 0 && pos.col < _width;
    return inside && movable(pos);
  }

  void visitTile(const Pos &pos) {

    get(pos)._containsPlayer = true;

    for (int i = -1; i <= 1; ++i) {
      for (int j = -1; j <= 1; ++j) {
        int r = pos.row + i;
        int c = pos.col + j;
        if (!validPosition(Pos(r,c)))
          continue;
        int dy = i < 0 ? -i : i;
        int dx = j < 0 ? -j : j;
        auto &tile = get(r, c);
        int v = min(2, max(0, 2 - (int)(sqrt(dx*dx+dy*dy) + 0.5f)));
        tile._visited += 64 * v;
        tile._visited = min(255, tile._visited);
        tile._sprite.setColor(sf::Color(tile._visited, tile._visited, tile._visited));
      }
    }

  }
private:
  vector<Tile> _tiles;
  vector<Monster> _monsters;
  int _width;
  int _height;
};

class PlayerFactory {
public:
  static bool create();
  static void close();
  static PlayerFactory &instance();

  Player *createPlayer(PlayerClass pc);
private:
  static PlayerFactory *_instance;  
};

PlayerFactory *PlayerFactory::_instance;

Player *PlayerFactory::createPlayer(PlayerClass pc) {
  Player *player = new Player();
  player->_class = pc;
  player->_vitality = (int)gaussianRand(10, 2);

  switch (pc) {

    case PlayerClass::kWizard: 
      player->_intelligence = (int)gaussianRand(20, 3); 
      player->_strength = (int)gaussianRand(10, 2); 
      player->_dexterity = (int)gaussianRand(10, 2); 
      break;
  
    case PlayerClass::kRogue: 
      player->_intelligence = (int)gaussianRand(10, 2); 
      player->_strength = (int)gaussianRand(10, 2); 
      player->_dexterity = (int)gaussianRand(20, 3); 
      break;

    case PlayerClass::kWarrior: 
      player->_intelligence = (int)gaussianRand(10, 2); 
      player->_strength = (int)gaussianRand(20, 3); 
      player->_dexterity = (int)gaussianRand(10, 2); 
      break;

    case PlayerClass::kCleric:
      player->_intelligence = (int)gaussianRand(20, 3); 
      player->_strength = (int)gaussianRand(10, 2); 
      player->_dexterity = (int)gaussianRand(10, 2); 
      break;
  }
  return player;
}

bool PlayerFactory::create() {
  assert(!_instance);
  _instance = new PlayerFactory();
  return true;
}

PlayerFactory &PlayerFactory::instance() {
  assert(_instance);
  return *_instance;
}

void PlayerFactory::close() {
  assert(_instance);
  delete exch_null(_instance);
}

class LevelFactory {

public:
  static bool create();
  static void close();
  static LevelFactory &instance();

  Level *createLevel(int width, int height, const sf::Texture &envTexture, const sf::Texture &charTexture);
private:
  static LevelFactory *_instance;
};

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
    Monster monster;
    int r = 1 + (rand() % (height-2));
    int c = 1 + (rand() % (width-2));
    monster._pos = Pos(r, c);
    auto &sprite = monster._sprite;
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


class Game {

public:
  enum GameState {
    WaitingForPlayer,
    WaitingForWorld,
  };

  void init() {
  }

private:

  GameState _state;
};

struct StateBase {

  virtual void update(const sf::Event &event) = 0;
};

#ifdef _WIN32
#define OVERRIDE override
#else
#define OVERRIDE
#endif
struct PlayerState : public StateBase {

  virtual void update(const sf::Event &event) OVERRIDE {

    auto &players = _party->_players;
    Player *player = players[_party->_curPlayer];
    bool newPos = false;
    bool skipPlayer = false;

    Pos oldPos = player->_pos;

    if (event.type == sf::Event::KeyReleased) {
      auto code = event.key.code;
      struct {
        sf::Keyboard::Key code;
        Pos ofs;
      } moves[] = {
        { sf::Keyboard::Left,   Pos(0,-1) },
        { sf::Keyboard::Right,  Pos(0,+1) },
        { sf::Keyboard::Up,     Pos(-1,0) },
        { sf::Keyboard::Down,   Pos(+1,0) },
      };

      for (int i = 0; i < ELEMS_IN_ARRAY(moves); ++i) {
        auto &cur = moves[i];
        if (code == cur.code) {
          if (_level->validPosition(player->_pos + cur.ofs)) {
            player->_pos = player->_pos + cur.ofs;
            newPos = true;
            break;
          }
        }
      }

      if (!newPos) {
        if (code == sf::Keyboard::Space) {
          skipPlayer = true;
        }
      }
    }
    
    if (newPos) {
      player->_hasMoved = true;
      _level->get(oldPos)._containsPlayer = false;
      _level->visitTile(player->_pos);
      player->_activePlayer = false;
    }

    // Set next player
    bool done = true;
    if (skipPlayer) {
      player->_activePlayer = false;
      _party->_curPlayer++;
    }

    for (size_t i = 0; i < players.size(); ++i) {
      int idx = (_party->_curPlayer + i) % players.size();
      Player *p = players[idx];
      if (!p->_hasMoved) {
        _party->_curPlayer = idx;
        p->_activePlayer = true;
        done = false;
        break;
      }
    }

    if (done) {
      for (size_t i = 0; i < players.size(); ++i) {
        players[i]->_activePlayer = false;
        players[i]->_hasMoved = false;
      }
      _party->_curPlayer = 0;
      players[0]->_activePlayer = true;
    }

  }

  Level *_level;
  Party *_party;
};

struct AiState : public StateBase {

  virtual void update(const sf::Event &event) OVERRIDE {

  }
};


class App {

public:

  bool close() {
    LevelFactory::close();
    PlayerFactory::close();

    delete exch_null(_level);
    delete exch_null(_window);
    return true;
  }

  bool init() {
    findAppRoot();

    _window = new sf::RenderWindow(sf::VideoMode(800, 600), "while (true) { kill(stuff); get(epix); }");

    if (!_environmentTexture.loadFromFile("oryx_lofi/lofi_environment.png"))
      return EXIT_FAILURE;

    if (!_characterTexture.loadFromFile("oryx_lofi/lofi_char.png"))
      return EXIT_FAILURE;

    if (!_font.loadFromFile("gfx/wscsnrg.ttf"))
      return EXIT_FAILURE;

    LevelFactory::create();
    PlayerFactory::create();

    _level = LevelFactory::instance().createLevel(20, 20, _environmentTexture, _characterTexture);

    _curState = &_playerState;

    for (int i = 0; i < 4; ++i) {
      auto *p = PlayerFactory::instance().createPlayer((PlayerClass)i);
      p->_sprite.setTexture(_characterTexture);
      p->_sprite.setScale(3, 3);
      p->_sprite.setTextureRect(sf::IntRect(0, 0, 8, 8));
      p->_pos = Pos(1 + (i+1) % 2, 1 + (i+1)/2);
      _level->visitTile(p->_pos);
      _party._players.push_back(p);
    }
    _party._players[_party._curPlayer]->_activePlayer = true;

    _playerState._party = &_party;
    _playerState._level = _level;

    return true;
  }

  void update(const sf::Event &event) {
    _curState->update(event);
  }

  void drawPartyStats() {

    float x = 500;
    float y = 0;
    sf::Text heading("", _font, 20);
    sf::Text normal("", _font, 10);

    sf::Vector2f pos(x, y);

    auto &drawHeading = [&](Player *player) {
      heading.setString(player->_name);
      heading.setPosition(pos);
      sf::FloatRect r = heading.getLocalBounds();
      sf::Vector2f tmpPos = pos;
      pos.x += r.width + 10;
      _window->draw(heading);
      normal.setString(toString("(%s)", playerClassToString(player->_class).c_str()));
      pos.y += (r.height - normal.getLocalBounds().height) / 2;
      normal.setPosition(pos);
      _window->draw(normal);
      pos = tmpPos;
      pos.y += 25;
    };

    auto &drawNormal = [&](const std::string &str) {
      normal.setString(str);
      normal.setPosition(pos);
      pos.y += 15;
      _window->draw(normal);
    };

    for (auto *player : _party._players) {

      heading.setColor(player->_activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));
      normal.setColor(player->_activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));

      drawHeading(player);

      drawNormal(toString("STR: %d", player->_strength));
      drawNormal(toString("INT: %d", player->_intelligence));
      drawNormal(toString("DEX: %d", player->_dexterity));
      drawNormal(toString("VIT: %d", player->_vitality));
    }
  }

  int run()
  {


    // Start the game loop
    while (_window->isOpen())
    {
      // Process events
      sf::Event event;
      while (_window->pollEvent(event)) {

        update(event);

        if (event.type == sf::Event::Closed)
          _window->close();
      }

      _window->clear();

      _level->draw(_window);

      for (size_t i = 0; i < _party._players.size(); ++i) {
        Player *cur = _party._players[i];
        cur->_name = toString("Player %d", i);
        cur->_sprite.setPosition((float)cur->_pos.col*3*8, (float)cur->_pos.row*3*8);
        cur->_sprite.setColor(cur->_activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));
        _window->draw(cur->_sprite);
      }

      drawPartyStats();

      _window->display();
    }

    return EXIT_SUCCESS;
  }

private:

  void findAppRoot()
  {
#ifdef _WIN32
    char startingDir[MAX_PATH];
    _getcwd(startingDir, MAX_PATH);

    // keep going up directory levels until we find "app.json", or we hit the bottom..
    char prevDir[MAX_PATH], curDir[MAX_PATH];
    ZeroMemory(prevDir, sizeof(prevDir));

    while (true) {
      _getcwd(curDir, MAX_PATH);
      // check if we haven't moved
      if (!strcmp(curDir, prevDir))
        break;

      memcpy(prevDir, curDir, MAX_PATH);

      if (fileExists("settings.ini")) {
        _appRoot = curDir;
        return;
      }

      if (_chdir("..") == -1)
        break;
    }
    _appRoot = startingDir;
#endif
  }

  Party _party;
  PlayerState _playerState;
  AiState _aiState;

  StateBase *_curState;

  Level *_level;

  sf::Font _font;

  sf::Texture _environmentTexture;
  sf::Texture _characterTexture;
  sf::RenderWindow *_window;

  string _appRoot;

};


int main() {
  srand(1337);

  App app;
  if (!app.init())
    return 1;

  int res = app.run();

  app.close();

  if (res) {
#ifdef _WIN32
    while (!isBitSet(GetAsyncKeyState(VK_ESCAPE), 15))
      ;
#endif
  }

  return res;
}
