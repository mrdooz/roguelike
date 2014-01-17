#include "editor.hpp"
#include "utils.hpp"
#include "animation_editor.hpp"
#include "window_event_manager.hpp"
#include "error.hpp"
#include "animation_manager.hpp"
#include "texture_cache.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <CoreGraphics/CGDirectDisplay.h>
#endif

using namespace rogue;

Editor *Editor::_instance;

//-----------------------------------------------------------------------------
Editor::Editor()
  : _window(nullptr)
  , _animationEditor(nullptr)
  , _windowEventManager(nullptr)
  , _animationManager(nullptr)
{
}

//-----------------------------------------------------------------------------
Editor::~Editor()
{
  delete exch_null(_animationManager);
  delete exch_null(_window);
  delete exch_null(_animationEditor);
  delete exch_null(_windowEventManager);
}

//-----------------------------------------------------------------------------
bool Editor::Close()
{
  assert(_instance);
  delete exch_null(_instance);
  return true;
}

//-----------------------------------------------------------------------------
bool Editor::Create()
{
  assert(!_instance);
  _instance = new Editor();
  return _instance->Init();
}

//-----------------------------------------------------------------------------
Editor* Editor::Instance()
{
  assert(_instance);
  return _instance;
}

//-----------------------------------------------------------------------------
WindowEventManager* Editor::GetWindowEventManager()
{
  return _windowEventManager;
}

//-----------------------------------------------------------------------------
bool Editor::OnResize(const Event& event)
{
  size_t width = event.size.width;
  size_t height = event.size.height;
  _window->setView(sf::View(sf::FloatRect(0,0,(float)width, (float)height)));
  return false;
}

//-----------------------------------------------------------------------------
bool Editor::InitMainWindow()
{
  size_t width, height;
#ifdef _WIN32
  width = GetSystemMetrics(SM_CXFULLSCREEN);
  height = GetSystemMetrics(SM_CYFULLSCREEN);
#else
  auto displayId = CGMainDisplayID();
  width = CGDisplayPixelsWide(displayId);
  height = CGDisplayPixelsHigh(displayId);
#endif

  _window = new RenderWindow(sf::VideoMode(8 * width / 10, 8 * height / 10), "...");
  _window->setFramerateLimit(60);

  // Note, to avoid bugs having to do with registering handlers pointing to NULL objects,
  // it's easier if classes register themselves in their ctors
  _windowEventManager = new WindowEventManager(_window);
  _windowEventManager->RegisterHandler(Event::Closed, [this](const Event&) { _window->close(); return true; });
  _windowEventManager->RegisterHandler(Event::Resized, bind(&Editor::OnResize, this, _1));

  _animationEditor = new AnimationEditor(_window, _windowEventManager, _animationManager);
  if (!_animationEditor->Init())
    return false;

  return true;
}

//-----------------------------------------------------------------------------
bool Editor::Init()
{
  srand(1337);

  FindAppRoot();

  if (!g_logSinkFile.Open("editor.log"))
    return 1;

  if (!TextureCache::Create())
    return false;

  _animationManager = new AnimationManager();
  if (!_animationManager->LoadAnimations("config/animation_config.pb"))
  {
    return false;
  }

  if (!InitMainWindow())
    return false;

  return true;
}

//-----------------------------------------------------------------------------
void Editor::ProcessMainWindow()
{
  _windowEventManager->Poll();

  _window->clear();
  _animationEditor->Update();
  _window->display();
}


//-----------------------------------------------------------------------------
int Editor::Run()
{
  // Start the game loop
  while (_window->isOpen())
  {
    TextureCache::Instance()->CheckForReload();
    _animationManager->CheckForReload();

    ProcessMainWindow();
  }

  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
void Editor::FindAppRoot()
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
AnimationManager* Editor::GetAnimationManager()
{
  return _animationManager;
}

//-----------------------------------------------------------------------------
int main()
{
  if (!Editor::Create())
    return 1;

  int res = Editor::Instance()->Run();
  Editor::Instance()->Close();
  return res;
}
