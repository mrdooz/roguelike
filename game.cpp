#include "precompiled.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "player_factory.hpp"
#include "level_factory.hpp"

using namespace std;

bool Game::close() {
  LevelFactory::close();
  PlayerFactory::close();

  delete exch_null(_party);
  delete exch_null(_level);
  delete exch_null(_window);
  return true;
}

bool Game::init() {
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

  _level = LevelFactory::instance().createLevel(80, 50, _environmentTexture, _characterTexture);
  _party = new Party;

  _curState = &_playerState;

  for (int i = 0; i < 4; ++i) {
    auto *p = PlayerFactory::instance().createPlayer((PlayerClass)i);
    p->_sprite.setTexture(_characterTexture);
    p->_sprite.setScale(3, 3);
    p->_sprite.setTextureRect(sf::IntRect(0, 0, 8, 8));
    p->_pos = Pos(1 + (i+1) % 2, 1 + (i+1)/2);
    _level->initPlayer(p, p->_pos);
    _party->_players.push_back(p);
  }
  //_party->_players[_party->_curPlayer]->_activePlayer = true;

  _playerState._party = _party;
  _playerState._level = _level;

  return true;
}

void Game::update(const sf::Event &event) {
  _curState->update(event);
}

void Game::drawPartyStats(int width) {

  auto size = _window->getSize();
  float x = (float)(size.x - width);
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

  auto *activePlayer = _party->_players[_party->_activePlayer];
  for (auto *player : _party->_players) {

    heading.setColor(player == activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));
    normal.setColor(player == activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));

    drawHeading(player);

    drawNormal(toString("STR: %d", player->_strength));
    drawNormal(toString("INT: %d", player->_intelligence));
    drawNormal(toString("DEX: %d", player->_dexterity));
    drawNormal(toString("VIT: %d", player->_vitality));
  }
}

int Game::run()
{

  sf::View view = _window->getDefaultView();

  // Start the game loop
  while (_window->isOpen())
  {
    // Process events
    sf::Event event;
    while (_window->pollEvent(event)) {

      update(event);

      if (event.type == sf::Event::Resized) {
        _window->setView(sf::View(sf::FloatRect(0,0,(float)event.size.width, (float)event.size.height)));
        int a = 10;
      } else if (event.type == sf::Event::Closed) {
        _window->close();
      }
    }

    _window->clear();

    Pos topLeft(_party->_players[0]->_pos);
    for (auto *player : _party->_players) {
      topLeft.row = min(topLeft.row, player->_pos.row);
      topLeft.col = min(topLeft.col, player->_pos.col);
    }

    auto size = _window->getSize();
    int partyWidth = 200;
    int zoom = 3;
    int tilesX = (size.x - partyWidth) / (zoom*8);
    int tilesY = (size.y) / (zoom*8);

    topLeft.row = max(0, topLeft.row - tilesY/2);
    topLeft.col = max(0, topLeft.col - tilesX/2);
    _level->draw(_window, topLeft, tilesY, tilesX);

    auto *activePlayer = _party->_players[_party->_activePlayer];
    for (size_t i = 0; i < _party->_players.size(); ++i) {
      Player *cur = _party->_players[i];
      cur->_name = toString("Player %d", i);
      cur->_sprite.setPosition((float)(cur->_pos.col - topLeft.col)*3*8, (float)(cur->_pos.row - topLeft.row)*3*8);
      cur->_sprite.setColor(cur == activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));
      _window->draw(cur->_sprite);
    }

    drawPartyStats(200);

    _window->display();
  }

  return EXIT_SUCCESS;
}

void Game::findAppRoot()
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
