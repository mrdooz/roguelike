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

using namespace rogue;

Game *Game::_instance;

Game::~Game() {
  LevelFactory::close();
  PlayerFactory::close();
  delete exch_null(_renderer);
  delete exch_null(_window);
}

bool Game::close() {
  assert(_instance);
  delete exch_null(_instance);
  return true;
}

bool Game::create() {
  assert(!_instance);
  _instance = new Game();
  return _instance->init();
}

Game &Game::instance() {
  assert(_instance);
  return *_instance;
}

bool Game::init()
{
  findAppRoot();

  _window = new sf::RenderWindow(sf::VideoMode(800, 600), "while (true) { kill(stuff); get(epix); }");

  LevelFactory::create();
  PlayerFactory::create();

  _gameState._level = LevelFactory::instance().CreateLevel(200, 200);
  _gameState._party = new Party();

  for (int i = 0; i < 4; ++i)
  {
    auto *p = PlayerFactory::instance().createPlayer((PlayerClass)i);
    p->_sprite.setScale(3, 3);
    p->_pos = Pos(1 + (i+1) % 2, 1 + (i+1)/2);
    _gameState._level->initPlayer(p, p->_pos);
    _gameState._party->_players.push_back(p);
  }
  _gameState._level->initMonsters();

  _renderer = new Renderer(_window);
  _renderer->init(_gameState);

  //_playerState.addMoveDoneListener(std::bind(&Renderer::onMoveDone, _renderer));

  return true;
}

int Game::run()
{

  sf::View view = _window->getDefaultView();

  // Start the game loop
  while (_window->isOpen())
  {
    // Process events
    sf::Event event;
    if (_window->pollEvent(event))
    {
      do
      {
        if (event.type == sf::Event::Resized)
        {
          _window->setView(sf::View(sf::FloatRect(0,0,(float)event.size.width, (float)event.size.height)));
        }
        else if (event.type == sf::Event::Closed)
        {
          _window->close();
        }
        else
        {
          if (event.type == sf::Event::KeyReleased)
          {
            UpdateState(_gameState, event.key.code);
          }
        }

      } while (_window->pollEvent(event));
    }
    else
    {
    }

    _window->clear();

    _renderer->drawWorld(_gameState);

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

void Game::addLogMessage(const char *fmt, ...) {

#ifdef _WIN32
  va_list arg;
  va_start(arg, fmt);

  const int len = _vscprintf(fmt, arg) + 1;

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, fmt, arg);
  va_end(arg);

  OutputDebugStringA(buf);
#endif
}
