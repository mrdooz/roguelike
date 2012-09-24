#include "precompiled.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "player_factory.hpp"
#include "level_factory.hpp"
#include "renderer.hpp"
#include "party.hpp"

using namespace std;

bool Game::close() {
  LevelFactory::close();
  PlayerFactory::close();

  delete exch_null(_party);
  delete exch_null(_level);
  delete exch_null(_renderer);
  delete exch_null(_window);
  return true;
}

bool Game::init() {
  findAppRoot();

  _window = new sf::RenderWindow(sf::VideoMode(800, 600), "while (true) { kill(stuff); get(epix); }");

  LevelFactory::create();
  PlayerFactory::create();

  _level = LevelFactory::instance().createLevel(80, 50);
  _party = new Party;

  _curState = &_playerState;

  for (int i = 0; i < 4; ++i) {
    auto *p = PlayerFactory::instance().createPlayer((PlayerClass)i);
    //p->_sprite.setTexture(_characterTexture);
    p->_sprite.setScale(3, 3);
    p->_pos = Pos(1 + (i+1) % 2, 1 + (i+1)/2);
    _level->initPlayer(p, p->_pos);
    _party->_players.push_back(p);
  }

  _playerState._party = _party;
  _playerState._level = _level;

  _renderer = new Renderer(_window);
  _renderer->init(_level, _party);

  return true;
}

void Game::update(const sf::Event &event) {
  _curState->update(event);
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

    _renderer->drawLevel();
    _renderer->drawParty();
    _renderer->drawPartyStats();

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
