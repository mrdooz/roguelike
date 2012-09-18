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

struct Player {
  Player() : _posX(0), _posY(0), _mode(PlayerMode::kInteractive), _hasMoved(false), _activePlayer(false) {}

  string _name;

  int _intelligence;
  int _strength;
  int _dexterity;
  int _vitality;

  int _health;
  int _mana;
  int _posX, _posY;

  bool _activePlayer;
  bool _hasMoved;

  PlayerMode _mode;
  PlayerClass _class;
};

struct Party {
  Party() : _curPlayer(0) {}
  int _curPlayer;
  vector<Player *> _players;
};

struct Tile {
  Tile() : _visited(0) {}
  sf::Sprite _sprite;
  int _visited;
};

struct Level {

  Tile &get(int row, int col) { return _tiles[row*_width+col]; }

  bool validPosition(int row, int col) {
    return row >= 0 && row < _height && col >= 0 && col < _width;
  }

  void visitTile(int row, int col) {

    for (int i = -1; i <= 1; ++i) {
      for (int j = -1; j <= 1; ++j) {
        int r = row + i;
        int c = col + j;
        if (!validPosition(r, c))
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

  vector<Tile> _tiles;
  int _width;
  int _height;
};

class LevelFactory {

public:
  static bool create();
  static void close();
  static LevelFactory &instance();

  Level *makeLevel(int width, int height, const sf::Texture &texture);
private:
  static LevelFactory *_instance;
};

Level *LevelFactory::makeLevel(int width, int height, const sf::Texture &texture) {
  Level *level = new Level;

  level->_width = width;
  level->_height = height;

  level->_tiles.resize(width*height);

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      auto &sprite = level->get(i, j)._sprite;
      sprite.setPosition((float)j*24, (float)i*24);
      sprite.setScale(3.0f, 3.0f);
      sprite.setColor(sf::Color(0,0,0,0));
      sprite.setTextureRect(sf::IntRect((int)Tiles::floorA*8, 0, 8, 8));
      sprite.setTexture(texture);
    }
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

    Player *player = _party->_players[_party->_curPlayer];
    bool newPos = false;

    if (event.type == sf::Event::KeyReleased) {
      if (event.key.code == sf::Keyboard::Left) {
        if (_level->validPosition(player->_posY, player->_posX-1)) {
          newPos = true;
          player->_posX--;
        }

      } else if (event.key.code == sf::Keyboard::Right) {
        if (_level->validPosition(player->_posY, player->_posX+1)) {
          newPos = true;
          player->_posX++;
        }

      } else if (event.key.code == sf::Keyboard::Up) {
        if (_level->validPosition(player->_posY-1, player->_posX)) {
          newPos = true;
          player->_posY--;
        }

      } else if (event.key.code == sf::Keyboard::Down) {
        if (_level->validPosition(player->_posY+1, player->_posX)) {
          newPos = true;
          player->_posY++;
        }
      }
    }
    
    if (newPos)
      _level->visitTile(player->_posY, player->_posX);

    // Check if all the players have moved
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
    _level = LevelFactory::instance().makeLevel(20, 20, _environmentTexture);

    _characterSprite.setTexture(_characterTexture);
    _characterSprite.setScale(3, 3);
    _characterSprite.setTextureRect(sf::IntRect(0, 0, 8, 8));

    _curState = &_playerState;

    for (int i = 0; i < 4; ++i) {
      auto *p = new Player();
      p->_posX = i / 2;
      p->_posY = i % 2;
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

      for (size_t i = 0; i < _level->_tiles.size(); ++i)
        _window->draw(_level->_tiles[i]._sprite);

      for (size_t i = 0; i < _party._players.size(); ++i) {
        Player *cur = _party._players[i];
        cur->_name = toString("Player %d", i);
        cur->_class = (PlayerClass)i;
        _characterSprite.setPosition((float)cur->_posX*3*8, (float)cur->_posY*3*8);
        _characterSprite.setColor(cur->_activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));
        _window->draw(_characterSprite);
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
  sf::Sprite _characterSprite;

  string _appRoot;

};


int main() {
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
