#include "window_event_manager.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
WindowEventManager::WindowEventManager(RenderWindow* window)
    : _nextId(1)
    , _window(window)
    , _keystate(1024, 0)
{
}

//-----------------------------------------------------------------------------
u32 WindowEventManager::RegisterHandler(Event::EventType event, const fnEventHandler& handler)
{
  _handlersByWindow[event].push_back(make_pair(_nextId, handler));

  // Store the id -> event mapping, so we can speed up unregistering
  _idToEvent[_nextId] = event;
  return _nextId++;
}

//-----------------------------------------------------------------------------
void WindowEventManager::UnregisterHandler(u32 handle)
{
  auto it = _idToEvent.find(handle);
  if (it != _idToEvent.end())
  {
    Event::EventType type = it->second;
    auto& handlers = _handlersByWindow[type];
    for (auto j = handlers.begin(); j != handlers.end(); ++j)
    {
      const HandlerPair& p = *j;
      if (p.first == handle)
      {
        handlers.erase(j);
        _idToEvent.erase(handle);
        return;
      }
    }
  }
}

//-----------------------------------------------------------------------------
bool IsModifierKey(Keyboard::Key key)
{
  return key == Keyboard::LShift || key == Keyboard::RShift
    || key == Keyboard::LAlt || key == Keyboard::RAlt
    || key == Keyboard::LControl || key == Keyboard::RControl;
}

//-----------------------------------------------------------------------------
// For the one liners, yo
Event MakeKeyEvent(Event::EventType type, Keyboard::Key key, bool shift, bool alt, bool control)
{
  Event event;
  event.type = type;
  event.key.code = key;
  event.key.shift = shift;
  event.key.alt = alt;
  event.key.control = control;
  event.key.system = false;
  return event;
}

//-----------------------------------------------------------------------------
void WindowEventManager::Poll()
{
  _window->setActive(true);

  // chec for changes in keystate
  bool shift  = Keyboard::isKeyPressed(Keyboard::LShift) || Keyboard::isKeyPressed(Keyboard::RShift);
  bool alt    = Keyboard::isKeyPressed(Keyboard::LAlt) || Keyboard::isKeyPressed(Keyboard::RAlt);
  bool ctrl   = Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl);

  // create upper 3 bits of the keyboard mask from the state of the modifier keys
  int modifierMask = (shift << 7) | (alt << 8) | (ctrl << 9);

  vector<Event> keypressed;
  vector<Event> keyreleased;
  Event event;
  event.key.alt = alt;
  event.key.shift = shift;
  event.key.control = ctrl;
  vector<u8> newState(1024);
  memset(newState.data(), 0, 1024);
  for (int i = 0; i < Keyboard::KeyCount; ++i)
  {
    Keyboard::Key key = (Keyboard::Key)(Keyboard::A + i);
    if (IsModifierKey(key))
      continue;

    u8 pressed  = Keyboard::isKeyPressed(key);

    // if the key isn't a modifier, we check for the key in combination with all permutations
    // of modifiers
    if (!shift && !alt && !ctrl && pressed && !_keystate[i])
      keypressed.push_back(MakeKeyEvent(Event::KeyPressed, key, false, false, false));
    else if (!shift && !alt && !ctrl && !pressed && _keystate[i])
      keyreleased.push_back(MakeKeyEvent(Event::KeyReleased, key, false, false, false));
    else if (shift && !alt && !ctrl && pressed && !_keystate[i | 128])
      keypressed.push_back(MakeKeyEvent(Event::KeyPressed, key, true, false, false));
    else if (shift && !alt && !ctrl && !pressed && _keystate[i | 128])
      keyreleased.push_back(MakeKeyEvent(Event::KeyReleased, key, true, false, false));
    else if (!shift && alt && !ctrl && pressed && !_keystate[i | 256])
      keypressed.push_back(MakeKeyEvent(Event::KeyPressed, key, false, true, false));
    else if (!shift && alt && !ctrl && !pressed && _keystate[i | 256])
      keyreleased.push_back(MakeKeyEvent(Event::KeyReleased, key, false, true, false));
    else if (shift && alt && !ctrl && pressed && !_keystate[i | 128 | 256])
      keypressed.push_back(MakeKeyEvent(Event::KeyPressed, key, true, true, false));
    else if (shift && alt && !ctrl && !pressed && _keystate[i | 128 | 256])
      keyreleased.push_back(MakeKeyEvent(Event::KeyReleased, key, true, true, false));
    else if (!shift && !alt && ctrl && pressed && !_keystate[i | 512])
      keypressed.push_back(MakeKeyEvent(Event::KeyPressed, key, false, false, true));
    else if (!shift && !alt && ctrl && !pressed && _keystate[i | 512])
      keyreleased.push_back(MakeKeyEvent(Event::KeyReleased, key, false, false, true));
    else if (shift && !alt && ctrl && pressed && !_keystate[i | 128 | 512])
      keypressed.push_back(MakeKeyEvent(Event::KeyPressed, key, true, false, true));
    else if (shift && !alt && ctrl && !pressed && _keystate[i | 128 | 512])
      keyreleased.push_back(MakeKeyEvent(Event::KeyReleased, key, true, false, true));
    else if (!shift && alt && ctrl && pressed && !_keystate[i | 256 | 512])
      keypressed.push_back(MakeKeyEvent(Event::KeyPressed, key, false, true, true));
    else if (!shift && alt && ctrl && !pressed && _keystate[i | 256 | 512])
      keyreleased.push_back(MakeKeyEvent(Event::KeyReleased, key, false, true, true));
    else if (shift && alt && ctrl && pressed && !_keystate[i | 128 | 256 | 512])
      keypressed.push_back(MakeKeyEvent(Event::KeyPressed, key, true, true, true));
    else if (shift && alt && ctrl && !pressed && _keystate[i | 128 | 256 | 512])
      keyreleased.push_back(MakeKeyEvent(Event::KeyReleased, key, true, true, true));

    newState[i | modifierMask] = pressed;
  }
  _keystate = newState;

  // Send keypressed/keyrelease events
  auto it = _handlersByWindow.find(Event::KeyPressed);
  if (it != _handlersByWindow.end())
  {
    const vector<HandlerPair>& handlers = it->second;
    for (const Event& e : keypressed)
    {
      for (const auto& p : handlers)
      {
        // Continue calling handlers until one returns true
        if (p.second(e))
          break;
      }
    }
  }

  it = _handlersByWindow.find(Event::KeyReleased);
  if (it != _handlersByWindow.end())
  {
    const vector<HandlerPair>& handlers = it->second;
    for (const Event& e : keyreleased)
    {
      for (const auto& p : handlers)
      {
        // Continue calling handlers until one returns true
        if (p.second(e))
          break;
      }
    }
  }


  // Poll the window for events, and dispatch any to out handlers
  while (_window->pollEvent(event))
  {
    // skip keypressed/released events
    if (event.type == Event::KeyPressed || event.type == Event::KeyReleased)
      continue;

    auto it = _handlersByWindow.find(event.type);
    if (it != _handlersByWindow.end())
    {
      auto& handlers = it->second;
      for (auto p : handlers)
      {
        // Continue calling handlers until one returns true
        if (p.second(event))
          break;
      }
    }
  }
}

