#include "game.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "player_factory.hpp"
#include "level_factory.hpp"
#include "renderer.hpp"
#include "party.hpp"
#include "window_event_manager.hpp"
#include "game_event_manager.hpp"
#include "error.hpp"
#include "game_player.hpp"
#include "game_ai.hpp"
#include "game_event_manager.hpp"
#include "animation_manager.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <CoreGraphics/CGDirectDisplay.h>
#endif

using namespace rogue;

Game *Game::_instance;

//-----------------------------------------------------------------------------
Game::Game()
    : _playerFactory(nullptr)
    , _gamePlayer(nullptr)
    , _gameAI(nullptr)
    , _window(nullptr)
    , _renderer(nullptr)
    , _windowEventManager(nullptr)
    , _gameEventManager(nullptr)
    , _animationManager(nullptr)
{
}

//-----------------------------------------------------------------------------
Game::~Game()
{
  delete exch_null(_animationManager);
  delete exch_null(_playerFactory);
  delete exch_null(_gamePlayer);
  delete exch_null(_gameAI);
  delete exch_null(_renderer);
  delete exch_null(_window);
  delete exch_null(_windowEventManager);
  delete exch_null(_gameEventManager);
}

//-----------------------------------------------------------------------------
bool Game::Close()
{
  assert(_instance);
  delete exch_null(_instance);
  return true;
}

//-----------------------------------------------------------------------------
bool Game::Create()
{
  assert(!_instance);
  _instance = new Game();
  return _instance->Init();
}

//-----------------------------------------------------------------------------
Game* Game::Instance()
{
  assert(_instance);
  return _instance;
}

//-----------------------------------------------------------------------------
WindowEventManager* Game::GetWindowEventManager()
{
  return _windowEventManager;
}

//-----------------------------------------------------------------------------
GameEventManager* Game::GetGameEventManager()
{
  return _gameEventManager;
}

//-----------------------------------------------------------------------------
GameState& Game::GetGameState()
{
  return _gameState;
}

//-----------------------------------------------------------------------------
bool Game::OnResize(const Event& event)
{
  size_t width = event.size.width;
  size_t height = event.size.height;
  _window->setView(sf::View(sf::FloatRect(0,0,(float)width, (float)height)));

  return false;
}

//-----------------------------------------------------------------------------
bool Game::Init()
{
  srand(1337);

  FindAppRoot();

  if (!g_logSinkFile.Open("rogue.log"))
    return 1;

  if (!TextureCache::Create())
    return false;

  _playerFactory = new PlayerFactory();
  _gameEventManager = new GameEventManager();
  _animationManager = new AnimationManager();
  if (!_animationManager->LoadAnimations("config/animation_config.pb"))
  {
    return false;
  }

  _gameState._level = LevelFactory::CreateLevel(1,30,30);
  _gameState.CreateParty(_playerFactory);

  size_t width, height;
#ifdef _WIN32
  width = GetSystemMetrics(SM_CXFULLSCREEN);
  height = GetSystemMetrics(SM_CYFULLSCREEN);
#else
  auto displayId = CGMainDisplayID();
  width = CGDisplayPixelsWide(displayId);
  height = CGDisplayPixelsHigh(displayId);
#endif

  _window = new sf::RenderWindow(sf::VideoMode(8 * width / 10, 8 * height / 10), "...");
  _window->setFramerateLimit(60);

  // Note, to avoid bugs having to do with registering handlers pointing to NULL objects,
  // it's easier if classes register themselves in their ctors
  _windowEventManager = new WindowEventManager(_window);
  _windowEventManager->RegisterHandler(Event::Closed, [this](const Event&) { _window->close(); return true; });
  _windowEventManager->RegisterHandler(Event::Resized, bind(&Game::OnResize, this, _1));

  _renderer = new Renderer(_window, _windowEventManager, _animationManager);
  if (!_renderer->Init(_gameState))
    return false;

  if (!_playerMessageFont.loadFromFile("gfx/wscsnrg.ttf"))
    return false;

  _gamePlayer = new GamePlayer(_windowEventManager, _renderer);
  _gameAI = new GameAI();

  _gamePlayer->Init();

  return true;
}


//-----------------------------------------------------------------------------
void Game::ProcessWindow()
{
  _windowEventManager->Poll();
  _gamePlayer->Update(_gameState);
  _gameAI->Update(_gameState);

  _window->clear();
  _renderer->DrawWorld(_gameState);

  auto now = microsec_clock::local_time();
  ProcessPlayerMessages(now);

  _window->display();
}

//-----------------------------------------------------------------------------
void Game::ProcessPlayerMessages(const ptime& now)
{
  Vector2f pos(10, 40);
  sf::Text text("", _playerMessageFont, 20);

  for (auto it = _playerMessages.begin(); it != _playerMessages.end(); )
  {
    auto& cur = *it;
    int64 msLeft = (cur._expiration - now).total_milliseconds();

    if (msLeft < 0)
    {
      it = _playerMessages.erase(it);
    }
    else
    {
      float s = min((int64)1000, msLeft) / 1000.0f;
      text.setString(cur._message);
      text.setPosition(pos);
      text.setColor(sf::Color((u8)(s*255), (u8)(s*255), (u8)(s*255), (u8)(s*255)));
      _window->draw(text);
      pos.y += 25;
      ++it;
    }
  }
}

//-----------------------------------------------------------------------------
int Game::Run()
{
  // Start the game loop
  while (_window->isOpen())
  {
    TextureCache::Instance()->CheckForReload();
    _animationManager->CheckForReload();

    ProcessWindow();
  }

  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
void Game::FindAppRoot()
{
#ifdef _WIN32
  char startingDir[MAX_PATH];
  if (!_getcwd(startingDir, MAX_PATH))
    return;

  // keep going up directory levels until we find "app.json", or we hit the bottom..
  char prevDir[MAX_PATH], curDir[MAX_PATH];
  ZeroMemory(prevDir, sizeof(prevDir));

  while (true) {
    if (!_getcwd(curDir, MAX_PATH))
      return;

    // check if we haven't moved
    if (!strncmp(curDir, prevDir, MAX_PATH))
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

//-----------------------------------------------------------------------------
void Game::AddPlayerMessage(const time_duration& duration, const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  _playerMessages.emplace_back(microsec_clock::local_time() + duration, ToString(fmt, args));
  va_end(args);
}

//-----------------------------------------------------------------------------
void Game::AddPlayerMessage(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  _playerMessages.emplace_back(microsec_clock::local_time() + seconds(5), ToString(fmt, args));
  va_end(args);
}


//-----------------------------------------------------------------------------
AnimationManager* Game::GetAnimationManager()
{
  return _animationManager;
}

//-----------------------------------------------------------------------------
u32 Game::RegisterSelectionListener(const fnSelectionListener& listener)
{
  _selectionListeners[_nextSelectionId] = listener;
  return _nextSelectionId++;
}


//-----------------------------------------------------------------------------
void Game::UnregisterSelectionListener(u32 id)
{
  _selectionListeners.erase(id);
}

//-----------------------------------------------------------------------------
void Game::PostSelectionEvent(const SelectionEvent& event)
{
  for (const auto& s : _selectionListeners)
  {
    s.second(event);
  }
}
