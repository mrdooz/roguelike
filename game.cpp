#include "precompiled.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "player_factory.hpp"
#include "level_factory.hpp"
#include "renderer.hpp"
#include "debug_renderer.hpp"
#include "party.hpp"

using namespace rogue;

Game *Game::_instance;

//-----------------------------------------------------------------------------
Game::~Game()
{
  LevelFactory::close();
  PlayerFactory::close();
  delete exch_null(_renderer);
  delete exch_null(_debugRenderer);
  delete exch_null(_window);
  delete exch_null(_debugWindow);
}

//-----------------------------------------------------------------------------
bool Game::close()
{
  assert(_instance);
  delete exch_null(_instance);
  return true;
}

//-----------------------------------------------------------------------------
bool Game::create()
{
  assert(!_instance);
  _instance = new Game();
  return _instance->init();
}

//-----------------------------------------------------------------------------
Game &Game::instance()
{
  assert(_instance);
  return *_instance;
}

//-----------------------------------------------------------------------------
void Game::CreateParty()
{
  _gameState._party = new Party();

  for (int i = 0; i < 4; ++i)
  {
    auto *p = PlayerFactory::instance().CreatePlayer((PlayerClass)i);
    p->_sprite.setScale(3, 3);
    while (true)
    {
      int x = rand() % _gameState._level->Width();
      int y = rand() % _gameState._level->Height();
      auto& tile = _gameState._level->Get(y, x);
      if (tile.IsEmpty() && tile._type == TileType::kFloor)
      {
        p->_pos = Pos(y, x);
        break;
      }
    }
    _gameState._level->initPlayer(p, p->_pos);
    _gameState._party->_players.push_back(p);
  }
}

//-----------------------------------------------------------------------------
bool Game::init()
{
  findAppRoot();

  _debugWindow = new sf::RenderWindow(sf::VideoMode(800, 600), "debug");
  _window = new sf::RenderWindow(sf::VideoMode(800, 600), "...");

  LevelFactory::create();
  PlayerFactory::create();

  _gameState._level = LevelFactory::instance().CreateLevel(40, 40);
  _gameState._level->initMonsters();

  CreateParty();

  _debugRenderer = new DebugRenderer(_debugWindow);
  if (!_debugRenderer->Init())
    return false;

  _renderer = new Renderer(_window);
  if (!_renderer->Init(_gameState))
    return false;

  return true;
}

//-----------------------------------------------------------------------------
void Game::ProcessMainWindow()
{
  _window->setActive();

  // Process events
  sf::Event event;
  while (_window->pollEvent(event))
  {
    sf::Event::EventType type = event.type;
    if (type == Event::Resized)
    {
      size_t width = event.size.width;
      size_t height = event.size.height;
      _window->setView(sf::View(sf::FloatRect(0,0,(float)width, (float)height)));
      _renderer->Resize(_gameState);
    }
    else if (type == Event::Closed)
    {
      _window->close();
    }
    else
    {
      UpdateState(_gameState, &event);
    }
  }

  UpdateState(_gameState, nullptr);

  _window->clear();
  _renderer->DrawWorld(_gameState);
  _window->display();
}

//-----------------------------------------------------------------------------
void Game::ProcessDebugWindow()
{
  _window->setActive();
  sf::Event event;
  while (_debugWindow->pollEvent(event))
  {
    _debugRenderer->Update(_gameState, &event);
  }

  _debugWindow->clear();
  _debugRenderer->Update(_gameState, nullptr);
  _debugWindow->display();
}

//-----------------------------------------------------------------------------
int Game::run()
{
  // Start the game loop
  while (_window->isOpen())
  {
    ProcessMainWindow();
    ProcessDebugWindow();
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
