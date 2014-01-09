#include "virtual_window_manager.hpp"
#include "virtual_window.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "window_event_manager.hpp"
#include "shapes.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
VirtualWindowManager::VirtualWindowManager(RenderWindow *window)
  : _renderWindow(window)
  , _nextId(1)
  , _nextDepth(1)
  , _focusWindow(nullptr)
{
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::LostFocus, bind(&VirtualWindowManager::OnLostFocus, this, _1));

  DEBUG_WINDOW_EVENT->RegisterHandler(Event::KeyReleased, bind(&VirtualWindowManager::OnKeyReleased, this, _1));

  DEBUG_WINDOW_EVENT->RegisterHandler(Event::MouseButtonPressed, bind(&VirtualWindowManager::OnMouseButtonPressed, this, _1));
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::MouseButtonReleased, bind(&VirtualWindowManager::OnMouseButtonReleased, this, _1));
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::MouseMoved, bind(&VirtualWindowManager::OnMouseMove, this, _1));
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::Resized, bind(&VirtualWindowManager::OnResize, this, _1));

  DEBUG_WINDOW_EVENT->RegisterHandler(Event::KeyReleased, bind(&VirtualWindowManager::HandlerForFocusWindow, this, _1));
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::MouseButtonReleased, bind(&VirtualWindowManager::HandlerForFocusWindow, this, _1));
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::MouseMoved, bind(&VirtualWindowManager::HandlerForFocusWindow, this, _1));

  _font.loadFromFile("gfx/wscsnrg.ttf");
}

//-----------------------------------------------------------------------------
VirtualWindowManager::~VirtualWindowManager()
{
  SeqDelete(&_windows);
}

//-----------------------------------------------------------------------------
bool VirtualWindowManager::OnKeyReleased(const Event& event)
{
  if (event.key.code == Keyboard::Escape)
    _movingWindow = nullptr;

  return false;
}

//-----------------------------------------------------------------------------
bool VirtualWindowManager::OnLostFocus(const Event& event)
{
  _movingWindow = nullptr;
  return false;
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

  int x0 = (int)window->_pos.x;
  int x1 = x0 + (int)window->_size.x;

  int y0 = (int)window->_pos.y;
  int y1 = y0 + (int)window->_size.y;

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
bool VirtualWindowManager::OnMouseButtonReleased(const Event& event)
{
  if (_movingWindow)
  {
    _movingWindow = nullptr;
    return false;
  }

  Vector2f pos = VectorCast<float>(sf::Mouse::getPosition(*_renderWindow));
  for (VirtualWindow* window : _windows)
  {
    // If the click is inside a new window, change focus
    FloatRect rect(window->_pos, window->_size);
    if (rect.contains(pos))
    {
      SetFocus(window);
      break;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
bool VirtualWindowManager::OnMouseMove(const Event& event)
{
  if (_movingWindow)
  {
    Vector2f mouseDelta = Vector2f((float)event.mouseMove.x, (float)event.mouseMove.y) - _startMovePos;
    _movingWindow->SetPosition(_windowOrgPos + mouseDelta);
  }
  return false;
}

//-----------------------------------------------------------------------------
bool VirtualWindowManager::OnMouseButtonPressed(const Event& event)
{
  Vector2f pos = VectorCast<float>(sf::Mouse::getPosition(*_renderWindow));
  for (VirtualWindow* window : _windows)
  {
    // If inside the title bar, start moving the window
    float w = (float)window->_borderWidth;
    float h = (float)window->_titleBarHeight;
    FloatRect titleRect(window->_pos - Vector2f(w, h), Vector2f(2*w + window->_size.x, h));

    if (titleRect.contains(pos))
    {
      // Save original window/mouse position to be able to update
      // deltas in mouse_mouse.
      _windowOrgPos = window->_pos;
      _startMovePos = pos;
      window->_moving = true;
      _movingWindow = window;
      break;
    }

    // If the click is inside a new window, change focus
    FloatRect rect(window->_pos, window->_size);
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
  window->_depth = _nextDepth++;

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
  // Sort windows according to depth (smallest is drawn first)
  sort(_windows.begin(), _windows.end(), [](const VirtualWindow* lhs, const VirtualWindow* rhs)
  { 
    return lhs->_depth < rhs->_depth;
  });

  for (VirtualWindow* window : _windows)
  {
    // draw window border
    float w = (float)window->_borderWidth;
    Vector2f size = VectorCast<float>(window->_size);
    sf::RoundedRectangleShape rect(size + Vector2f(2*w, w+10), 10, 10, 0, 0, 40);
    rect.setPosition(window->_pos - Vector2f(w, 10));
    rect.setFillColor(Color(43, 43, 43));

    // center the text (taking border into account)
    Text text(window->_title, _font, 8);
    float tw = text.getLocalBounds().width;
    text.setPosition(window->_pos + Vector2f(w + (size.x - tw) / 2, -10));
    text.setColor(Color::White);
    _renderWindow->draw(rect);
    _renderWindow->draw(text);

    window->Draw();
    _renderWindow->draw(window->_sprite);
  }
}
