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
#include "event_manager.hpp"
#include "error.hpp"
#include "game_player.hpp"
#include "game_ai.hpp"

#define USE_DEBUG_WINDOW

using namespace rogue;

Game *Game::_instance;

//-----------------------------------------------------------------------------
Game::Game()
    : _gamePlayer(nullptr)
    , _gameAI(nullptr)
    , _window(nullptr)
    , _debugWindow(nullptr)
    , _renderer(nullptr)
    , _debugRenderer(nullptr)
    , _eventManager(nullptr)
{
}

//-----------------------------------------------------------------------------
Game::~Game()
{
  LevelFactory::close();
  PlayerFactory::close();
  delete exch_null(_gamePlayer);
  delete exch_null(_gameAI);
  delete exch_null(_renderer);
  delete exch_null(_window);
#ifdef _USE_DEBUG_WINDOW
  delete exch_null(_debugRenderer);
  delete exch_null(_debugWindow);
#endif
  delete exch_null(_eventManager);
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
bool Game::OnMouseMove(const Event& event)
{
  return false;
}

//-----------------------------------------------------------------------------
bool Game::OnResized(const Event& event)
{
  size_t width = event.size.width;
  size_t height = event.size.height;
  _window->setView(sf::View(sf::FloatRect(0,0,(float)width, (float)height)));
  _renderer->Resize(_gameState);

  return true;
}

//-----------------------------------------------------------------------------
bool Game::InitMainWindow()
{
  _window = new sf::RenderWindow(sf::VideoMode(800, 600), "...");
  _eventManager = new EventManager(_window);

  _eventManager->RegisterHandler(Event::MouseMoved, bind(&Game::OnMouseMove, this, _1));
  _eventManager->RegisterHandler(Event::Resized, bind(&Game::OnResized, this, _1));
  _eventManager->RegisterHandler(Event::Closed, [this](const Event&) { _window->close(); return true; });
  _eventManager->RegisterHandler(Event::KeyReleased, bind(&GamePlayer::OnKeyPressed, _gamePlayer, std::ref(_gameState), _1));

  _renderer = new Renderer(_window);
  if (!_renderer->Init(_gameState))
    return false;

  return true;
}

//-----------------------------------------------------------------------------
bool Game::InitDebugWindow()
{
  _debugWindow = new sf::RenderWindow(sf::VideoMode(800, 600), "debug");
  _debugRenderer = new DebugRenderer(_debugWindow);
  if (!_debugRenderer->Init())
    return false;

  return true;
}

//-----------------------------------------------------------------------------
bool Game::init()
{
  findAppRoot();

  LevelFactory::create();
  PlayerFactory::create();

  _gameState._level = LevelFactory::instance().CreateLevel(40, 40);
  _gameState._level->initMonsters();

  _gamePlayer = new GamePlayer();
  _gameAI = new GameAI();

  CreateParty();

#ifdef USE_DEBUG_WINDOW
  if (!InitDebugWindow())
    return false;
#endif

  if (!InitMainWindow())
    return false;

  return true;
}


//-----------------------------------------------------------------------------
void Game::ProcessMainWindow()
{
  _window->setActive();

  _eventManager->Update();
  _gamePlayer->Update(_gameState);
  _gameAI->Update(_gameState);

  _window->clear();
  _renderer->DrawWorld(_gameState);
  _window->display();
}

//-----------------------------------------------------------------------------
void Game::ProcessDebugWindow()
{
#ifdef USE_DEBUG_WINDOW
  _debugWindow->setActive();
  sf::Event event;
  while (_debugWindow->pollEvent(event))
  {
    _debugRenderer->Update(_gameState, &event);
  }

  _debugWindow->clear();
  _debugRenderer->Update(_gameState, nullptr);
  _debugWindow->display();
#endif
}

//-----------------------------------------------------------------------------
int Game::run()
{
  // Start the game loop
  while (_window->isOpen())
  {
    if (_debugWindow && _debugRenderer)
      ProcessDebugWindow();

    if (_window && _renderer)
      ProcessMainWindow();
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
#else
  char startingDir[256];
  getcwd(startingDir, 256);

  // keep going up directory levels until we find "app.json", or we hit the bottom..
  char prevDir[256], curDir[256];
  memset(prevDir, 0, sizeof(prevDir));

  while (true) {
    getcwd(curDir, 256);
    // check if we haven't moved
    if (!strcmp(curDir, prevDir))
      break;

    memcpy(prevDir, curDir, 256);

    if (fileExists("settings.ini")) {
      _appRoot = curDir;
      return;
    }

    if (chdir("..") == -1)
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
