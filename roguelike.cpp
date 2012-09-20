#include "precompiled.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "player_factory.hpp"
#include "level_factory.hpp"

using namespace std;


struct StateBase {

  virtual void update(const sf::Event &event) = 0;
};

struct PlayerState : public StateBase {

  virtual void update(const sf::Event &event) OVERRIDE {

    auto &players = _party->_players;
    Player *player = players[_party->_curPlayer];
    bool movePlayer = false;
    bool skipPlayer = false;

    Pos newPos;
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
        newPos = Pos(player->_pos + cur.ofs);
        if (code == cur.code) {
          if (_level->validDestination(newPos)) {
            movePlayer = true;
            break;
          }
        }
      }

      if (!movePlayer) {
        if (code == sf::Keyboard::Space) {
          skipPlayer = true;
        }
      }
    }
    
    if (movePlayer) {
      player->_hasMoved = true;
      player->_activePlayer = false;
      _level->movePlayer(player, oldPos, newPos);
      player->_pos = newPos;
    }

    if (skipPlayer) {
      player->_activePlayer = false;
      _party->_curPlayer++;
    }

    // Set next player
    bool done = true;
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

    _level = LevelFactory::instance().createLevel(100, 100, _environmentTexture, _characterTexture);

    _curState = &_playerState;

    for (int i = 0; i < 4; ++i) {
      auto *p = PlayerFactory::instance().createPlayer((PlayerClass)i);
      p->_sprite.setTexture(_characterTexture);
      p->_sprite.setScale(3, 3);
      p->_sprite.setTextureRect(sf::IntRect(0, 0, 8, 8));
      p->_pos = Pos(1 + (i+1) % 2, 1 + (i+1)/2);
      _level->initPlayer(p, p->_pos);
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

      Pos topLeft(_party._players[0]->_pos);
      for (auto *player : _party._players) {
        topLeft.row = min(topLeft.row, player->_pos.row);
        topLeft.col = min(topLeft.col, player->_pos.col);
      }

      int windowWidth = 20;
      int windowHeight = 20;

      topLeft.row = max(0, topLeft.row - windowHeight/2);
      topLeft.col = max(0, topLeft.col - windowWidth/2);
      _level->draw(_window, topLeft, windowHeight, windowWidth);

      for (size_t i = 0; i < _party._players.size(); ++i) {
        Player *cur = _party._players[i];
        cur->_name = toString("Player %d", i);
        cur->_sprite.setPosition((float)(cur->_pos.col - topLeft.col)*3*8, (float)(cur->_pos.row - topLeft.row)*3*8);
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
