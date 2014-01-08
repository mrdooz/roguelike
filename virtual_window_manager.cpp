#include "virtual_window_manager.hpp"
#include "virtual_window.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "window_event_manager.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
VirtualWindowManager::VirtualWindowManager(RenderWindow *window)
  : _renderWindow(window)
  , _nextId(1)
  , _focusWindow(nullptr)
{
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::MouseButtonReleased, bind(&VirtualWindowManager::OnMouseButtonDown, this, _1));
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::Resized, bind(&VirtualWindowManager::OnResize, this, _1));

  DEBUG_WINDOW_EVENT->RegisterHandler(Event::KeyReleased, bind(&VirtualWindowManager::HandlerForFocusWindow, this, _1));
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::MouseButtonReleased, bind(&VirtualWindowManager::HandlerForFocusWindow, this, _1));
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::MouseMoved, bind(&VirtualWindowManager::HandlerForFocusWindow, this, _1));
}

//-----------------------------------------------------------------------------
VirtualWindowManager::~VirtualWindowManager()
{
  SeqDelete(&_windows);
}

//-----------------------------------------------------------------------------
bool VirtualWindowManager::OnResize(const Event& event)
{
  return false;
}

//-----------------------------------------------------------------------------
bool VirtualWindowManager::PointOnBorder(const VirtualWindow* window, int x, int y) const
{
  int w = window->_borderWidth;

  int x0 = window->_pos.x;
  int x1 = x0 + window->_size.x;

  int y0 = window->_pos.y;
  int y1 = y0 + window->_size.y;

  // Check if the mouse position is inside the window's border
  if ( (x >= x0 && x < x0 + w && y >= y0 && y < y1)
    || (y >= y0 && y < y0 + w && x >= x0 && x < x1)
    || (x >= x1 - w && x < x1 && y >= y0 && y < y1)
    || (y >= y1 - w && y < y1 && x >= x0 && x < x1))
  {
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
bool VirtualWindowManager::OnMouseButtonDown(const Event& event)
{
  // If the click is inside a new window, change focus

  Vector2i pos = sf::Mouse::getPosition(*_renderWindow);
  int x = pos.x;
  int y = pos.y;

  for (VirtualWindow* window : _windows)
  {

    Rect rect(VectorCast<int>(window->_pos), VectorCast<int>(window->_size));
    if (rect.contains(pos))
    {
      SetFocus(window);
      break;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
void VirtualWindowManager::SetFocus(VirtualWindow* window)
{
  if (_focusWindow)
    _focusWindow->_focus = false;
  _focusWindow = window;
  _focusWindow->_focus = true;
}

//-----------------------------------------------------------------------------
bool VirtualWindowManager::HandlerForFocusWindow(const Event& event)
{
  if (!_focusWindow)
    return false;

  // Check for any event handlers for the event
  auto itWindow = _handlers.find(event.type);
  if (itWindow == _handlers.end())
    return false;

  // Check if the focus window has a handler
  auto itHandler = itWindow->second.find(_focusWindow);
  if (itHandler == itWindow->second.end())
    return false;

  // Call the handler
  return itHandler->second.second(event);
}

//-----------------------------------------------------------------------------
void VirtualWindowManager::AddWindow(VirtualWindow* window)
{
  assert(!window->_windowManager);
  window->_windowManager = this;

  if (!_focusWindow)
    SetFocus(window);

  _windows.push_back(window);
}

//-----------------------------------------------------------------------------
size_t VirtualWindowManager::RegisterHandler(
    Event::EventType event,
    VirtualWindow* window,
    const fnEventHandler& handler)
{
  _handlers[event][window] = make_pair(_nextId, handler);
  return _nextId++;
}

//-----------------------------------------------------------------------------
void VirtualWindowManager::UnregisterHandler(size_t handle)
{
  for (auto& h : _handlers)
  {
    for (auto it = h.second.begin(); it != h.second.end(); ++it)
    {
      if (it->second.first == handle)
      {
        h.second.erase(it);
        return;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void VirtualWindowManager::Update()
{
  for (VirtualWindow* window : _windows)
  {
    window->Draw();
    _renderWindow->draw(window->_sprite);
  }

}
