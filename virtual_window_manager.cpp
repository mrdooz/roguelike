#include "virtual_window_manager.hpp"
#include "virtual_window.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "window_event_manager.hpp"
#include "shapes.hpp"
#include "standard_cursor.hpp"

using namespace rogue;
using namespace sf;

//-----------------------------------------------------------------------------
VirtualWindowManager::VirtualWindowManager(
    RenderWindow* window,
    WindowEventManager* eventManager)
  : _renderWindow(window)
  , _eventManager(eventManager)
  , _nextId(1)
  , _nextDepth(1)
  , _focusWindow(nullptr)
  , _movingWindow(nullptr)
  , _resizingWindow(nullptr)
  , _resizeFlags(0)
{
  _eventManager->RegisterHandler(Event::LostFocus, bind(&VirtualWindowManager::OnLostFocus, this, _1));

  _eventManager->RegisterHandler(Event::KeyReleased, bind(&VirtualWindowManager::OnKeyReleased, this, _1));

  _eventManager->RegisterHandler(Event::MouseButtonPressed, bind(&VirtualWindowManager::OnMouseButtonPressed, this, _1));
  _eventManager->RegisterHandler(Event::MouseButtonReleased, bind(&VirtualWindowManager::OnMouseButtonReleased, this, _1));
  _eventManager->RegisterHandler(Event::MouseMoved, bind(&VirtualWindowManager::OnMouseMove, this, _1));
  _eventManager->RegisterHandler(Event::Resized, bind(&VirtualWindowManager::OnResize, this, _1));

  _eventManager->RegisterHandler(Event::KeyReleased, bind(&VirtualWindowManager::HandlerForFocusWindow, this, _1));
  _eventManager->RegisterHandler(Event::MouseButtonReleased, bind(&VirtualWindowManager::HandlerForFocusWindow, this, _1));
  _eventManager->RegisterHandler(Event::MouseMoved, bind(&VirtualWindowManager::HandlerForFocusWindow, this, _1));

}

//-----------------------------------------------------------------------------
VirtualWindowManager::~VirtualWindowManager()
{
  SeqDelete(&_windows);
}

//-----------------------------------------------------------------------------
bool VirtualWindowManager::ResetMovingAndResizing()
{
  if (_movingWindow)
  {
    _movingWindow->_moving = false;
    _movingWindow = nullptr;
    return true;
  }

  if (_resizingWindow)
  {
    _resizingWindow->_resizing = false;
    _resizingWindow = nullptr;
    _resizeFlags = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
bool VirtualWindowManager::OnKeyReleased(const Event& event)
{
  if (event.key.code == Keyboard::Escape)
    ResetMovingAndResizing();

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
bool VirtualWindowManager::OnMouseButtonReleased(const Event& event)
{
  if (ResetMovingAndResizing())
    return false;

  Vector2f pos((float)event.mouseButton.x, (float)event.mouseButton.y);
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
  Vector2f pos((float)event.mouseMove.x, (float)event.mouseMove.y);

  if (_movingWindow)
  {
    Vector2f mouseDelta = pos - _startOperationPos;
    _movingWindow->SetPosition(_windowOrgPos + mouseDelta);
  }
  else if (_resizingWindow)
  {
    Vector2f mouseDelta = pos - _startOperationPos;
    Vector2f finalSize(_windowOrgSize);
    Vector2f finalPos(_windowOrgPos);

    if (!!(_resizeFlags & VirtualWindow::RightBorder))
    {
      finalSize.x += mouseDelta.x;
    }

    if (!!(_resizeFlags & VirtualWindow::LeftBorder))
    {
      // on left/top borders, we need to adjust both the position
      // and the size
      finalPos.x += mouseDelta.x;
      finalSize.x -= mouseDelta.x;
    }

    if (!!(_resizeFlags & VirtualWindow::BottomBorder))
    {
      finalSize.y += mouseDelta.y;
    }

    if (!!(_resizeFlags & VirtualWindow::TopBorder))
    {
      finalPos.y += mouseDelta.y;
      finalSize.y -= mouseDelta.y;
    }

    _resizingWindow->SetSize(finalSize);

    if (!!(_resizeFlags & (VirtualWindow::LeftBorder | VirtualWindow::TopBorder)))
      _resizingWindow->SetPosition(finalPos);

    // Send a resize event to the window
    Event resizeEvent;
    resizeEvent.type = Event::Resized;
    resizeEvent.size.width = (int)finalSize.x;
    resizeEvent.size.height = (int)finalSize.y;
    HandlerForFocusWindow(resizeEvent);
  }

  // Check if we should change the cursor due to being over a window corner
  if (_focusWindow)
  {
    u32 mask    = _focusWindow->PointInsideBorder(pos);
    bool top    = !!((mask & ~VirtualWindow::TopBorder) & VirtualWindow::TopBorderOuter);
    bool bottom = !!(mask & VirtualWindow::BottomBorder);
    bool left   = !!(mask & VirtualWindow::LeftBorder);
    bool right  = !!(mask & VirtualWindow::RightBorder);

    if ((top || bottom) && !left && !right)
    {
      StandardCursor::Set(StandardCursor::Type::SizeNS);
    }
    else if ((left || right) && !top && !bottom)
    {
      StandardCursor::Set(StandardCursor::Type::SizeWE);
    }
    else if ((top && left) || (bottom && right))
    {
      StandardCursor::Set(StandardCursor::Type::SizeNWSE);
    }
    else if ((top && right) || (bottom && left))
    {
      StandardCursor::Set(StandardCursor::Type::SizeNESW);
    }
    else
    {
      StandardCursor::Set(StandardCursor::Type::Normal);
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
bool VirtualWindowManager::OnMouseButtonPressed(const Event& event)
{
  Vector2f pos((float)event.mouseButton.x, (float)event.mouseButton.y);
  for (VirtualWindow* window : _windows)
  {
    float w = (float)window->_borderWidth;
    float h = (float)window->_titleBarHeight;

    // If the click is inside a new window, change focus
    if (_focusWindow != window)
    {
      if (window->PointInside(pos, true))
        SetFocus(window);
    }

    // If inside the title bar, start moving the window
    u32 mask = window->PointInsideBorder(pos);
    if (!!(mask & VirtualWindow::TopBorder) && !(mask & VirtualWindow::TopBorderOuter))
    {
      // Save original window/mouse position to be able to update
      // deltas in mouse_mouse.
      _windowOrgPos = window->_pos;
      _startOperationPos = pos;
      _movingWindow = window;
      _movingWindow->_moving = true;
      break;
    }
    else if (mask > 0)
    {
      // inside one of the resize borders
      _windowOrgSize = window->_size;
      _windowOrgPos = window->_pos;
      _startOperationPos = pos;
      _resizingWindow = window;
      _resizeFlags = mask;
      _resizingWindow->_resizing = true;
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

  // If the event is a mouse event, remap the coordinates to the virtual
  // window
  int w = (int)_focusWindow->_size.x;
  int h = (int)_focusWindow->_size.y;

  Event remappedEvent(event);
  if (event.type == Event::MouseButtonPressed)
  {
    remappedEvent.mouseButton.x -= (int)_focusWindow->_pos.x;
    remappedEvent.mouseButton.y -= (int)_focusWindow->_pos.y;
    remappedEvent.mouseButton.x = Clamp(remappedEvent.mouseButton.x, 0, w - 1);
    remappedEvent.mouseButton.y = Clamp(remappedEvent.mouseButton.y, 0, h - 1);
  }
  else if (event.type == Event::MouseButtonReleased)
  {
    remappedEvent.mouseButton.x -= (int)_focusWindow->_pos.x;
    remappedEvent.mouseButton.y -= (int)_focusWindow->_pos.y;
    remappedEvent.mouseButton.x = Clamp(remappedEvent.mouseButton.x, 0, w - 1);
    remappedEvent.mouseButton.y = Clamp(remappedEvent.mouseButton.y, 0, h - 1);
  }
  else if (event.type == Event::MouseMoved)
  {
    remappedEvent.mouseMove.x -= (int)_focusWindow->_pos.x;
    remappedEvent.mouseMove.y -= (int)_focusWindow->_pos.y;
    remappedEvent.mouseMove.x = Clamp(remappedEvent.mouseMove.x, 0, w - 1);
    remappedEvent.mouseMove.y = Clamp(remappedEvent.mouseMove.y, 0, h - 1);
  }

  // Call the handler
  return itHandler->second.second(remappedEvent);
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
bool VirtualWindowManager::Init()
{
  for (VirtualWindow* window : _windows)
  {
    if (!window->Init())
      return false;
  }

  return true;
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
    window->DrawBorder(_renderWindow);
    if (!window->_resizing)
    {
      window->Draw();
      _renderWindow->draw(window->_sprite);
    }
  }
}
