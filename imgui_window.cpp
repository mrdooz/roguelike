#include "imgui_window.hpp"
#include "virtual_window_manager.hpp"

using namespace rogue;
using namespace sf;

namespace ButtonMask
{
  enum Type
  {
    Left    = 1 << 1,
    Right   = 1 << 2,
    Middle  = 1 << 3,
  };
}

namespace ModifierMask
{
  enum Type
  {
    Shift   = 1 << 1,
    Alt     = 1 << 2,
    Control = 1 << 3,
  };
}

#ifdef IMGUI_SRC_ID
#define GEN_ID ((IMGUI_SRC_ID) + (__LINE__))
#else
#define GEN_ID (__LINE__)
#endif

//-----------------------------------------------------------------------------
UIState::UIState()
  : mouseX(0)
  , mouseY(0)
  , mouseDown(0)
  , hotItem(0)
  , activeItem(0)
  , keyboardItem(0)
  , key(Keyboard::Unknown)
  , keyMod(0)
  , lastItem(0)
  , keyChar(0)
{
}

//-----------------------------------------------------------------------------
ImGuiWindow::ImGuiWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
  : VirtualWindow(title, pos, size)
{
}

//-----------------------------------------------------------------------------
bool ImGuiWindow::Init()
{
  if (!_font.loadFromFile("gfx/wscsnrg.ttf"))
    return false;

  _windowManager->RegisterHandler(Event::MouseMoved, this, bind(&ImGuiWindow::OnMouseMove, this, _1));
  _windowManager->RegisterHandler(Event::MouseButtonPressed, this, bind(&ImGuiWindow::OnMouseButtonPressed, this, _1));
  _windowManager->RegisterHandler(Event::MouseButtonReleased, this, bind(&ImGuiWindow::OnMouseButtonReleased, this, _1));
  _windowManager->RegisterHandler(Event::KeyPressed, this, bind(&ImGuiWindow::OnKeyPressed, this, _1));
  _windowManager->RegisterHandler(Event::KeyReleased, this, bind(&ImGuiWindow::OnKeyReleased, this, _1));
  _windowManager->RegisterHandler(Event::TextEntered, this, bind(&ImGuiWindow::OnTextEntered, this, _1));
  return true;
}

//-----------------------------------------------------------------------------
void ImGuiWindow::ImGuiPrepare()
{
  _uistate.hotItem = 0;
}

//-----------------------------------------------------------------------------
void ImGuiWindow::ImGuiFinish()
{
  if (_uistate.mouseDown == 0)
  {
    _uistate.activeItem = 0;
  }
  else
  {
    // Mouse down outside the widgets, so set active to something invalid
    // to avoid widgets incorrectly thinking they got pressed
    if (_uistate.activeItem == 0)
      _uistate.activeItem = 1;
  }

  // If no widget grabbed tab, clear focus
  if (_uistate.key == Keyboard::Tab)
    _uistate.keyboardItem = 0;

  // Clear the entered key
  _uistate.key = Keyboard::Unknown;
  _uistate.keyChar = 0;
}

//-----------------------------------------------------------------------------
void ImGuiWindow::Draw()
{
  ImGuiPrepare();

  _texture.clear();

  Button(GEN_ID,50,50, 10, 10);
  Button(GEN_ID,150,50, 10, 10);

  static char sometext[80] = "Some text";
  TextField(GEN_ID, 50, 250, 40, 10, sometext);

  _texture.display();

  ImGuiFinish();
}

//-----------------------------------------------------------------------------
bool ImGuiWindow::RegionHit(int x, int y, int w, int h)
{
  if (_uistate.mouseX < x
    || _uistate.mouseY < y
    || _uistate.mouseX >= x + w
    || _uistate.mouseY >= y + h)
  {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
void ImGuiWindow::DrawRect(int x, int y, int w, int h, const Color& color)
{
  RectangleShape r(Vector2f(w, h));
  r.setPosition(x, y);
  r.setFillColor(color);
  _texture.draw(r);
}

//-----------------------------------------------------------------------------
void ImGuiWindow::Label(int x, int y, int h, const char* str)
{
  Text text("", _font, h);
  text.setString(str);
  text.setPosition(x, y);
  _texture.draw(text);
}

//-----------------------------------------------------------------------------
bool ImGuiWindow::Button(int id, int x, int y, int w, int h)
{
  if (RegionHit(x, y, w, h))
  {
    _uistate.hotItem = id;
    if (_uistate.activeItem == 0 && (_uistate.mouseDown & ButtonMask::Left))
      _uistate.activeItem = id;
  }

  // If no widget has keyboard focus, take it
  if (_uistate.keyboardItem == 0)
    _uistate.keyboardItem = id;

  if (_uistate.keyboardItem == id)
    DrawRect(x-2, y-2, w+4, h+4, Color(80, 0, 0));

  if (_uistate.hotItem == id)
  {
    if (_uistate.activeItem == id)
    {
      // hot and active
      DrawRect(x+2, y+2, w, h, Color(0xff, 0xff, 0xff));
    }
    else
    {
      // hot
      DrawRect(x, y, w, h, Color(0xcc, 0xcc, 0xcc));
    }
  }
  else
  {
    // button isn't hot
    DrawRect(x, y, w, h, Color(0xaa, 0xaa, 0xaa));
  }

  // Check for keyboard commands
  if (_uistate.keyboardItem == id)
  {
    switch (_uistate.key)
    {
    case Keyboard::Tab:
      // If tab is pressed, lose keyboard focus.
      // Next widget will grab the focus.
      _uistate.keyboardItem = 0;
      // If shift was also pressed, we want to move focus
      // to the previous widget instead.
      if (_uistate.keyMod & ModifierMask::Shift)
        _uistate.keyboardItem = _uistate.lastItem;
      // Also clear the key so that next widget
      // won't process it
      _uistate.key = Keyboard::Unknown;
      break;

    case Keyboard::Return:
      // Had keyboard focus, received return,
      // so we'll act as if we were clicked.
      return true;
    }
  }

  _uistate.lastItem = id;

  // If button is hot and active, but mouse button isn't down,
  // the user must have pressed the button
  if (_uistate.activeItem == id && _uistate.hotItem == id
    && (_uistate.mouseDown & ButtonMask::Left) == 0)
    return true;

  return false;
}

//-----------------------------------------------------------------------------
bool ImGuiWindow::TextField(int id, int x, int y, int maxLen, int h, char* buffer)
{
  int len = strlen(buffer);
  bool changed = false;

  // Check for hotness
  if (RegionHit(x-4, y-4, maxLen*h+8, h+10+8))
  {
    _uistate.hotItem = id;
    if (_uistate.activeItem == 0 && (_uistate.mouseDown & ButtonMask::Left))
      _uistate.activeItem = id;
  }

  if (_uistate.keyboardItem == 0)
    _uistate.keyboardItem = id;

  if (_uistate.keyboardItem == id)
    DrawRect(x-6, y-6, maxLen*h+12, h+10+12, Color(80, 0, 0));

  if (_uistate.activeItem == id || _uistate.hotItem == id)
    DrawRect(x-4, y-4, maxLen*h+8, h+10+8, Color(0xaa, 0xaa, 0xaa));
  else
    DrawRect(x-4, y-4, maxLen*h+8, h+10+8, Color(0x77, 0x77, 0x77));

  Label(x, y, h, buffer);

  // If we have keyboard focus, we'll need to process the keys
  if (_uistate.keyboardItem == id)
  {
    switch (_uistate.key)
    {
    case Keyboard::Tab:
      // If tab is pressed, lose keyboard focus.
      // Next widget will grab the focus.
      _uistate.keyboardItem = 0;
      // If shift was also pressed, we want to move focus
      // to the previous widget instead.
      if (_uistate.keyMod & ModifierMask::Shift)
        _uistate.keyboardItem = _uistate.lastItem;
      // Also clear the key so that next widget
      // won't process it
      _uistate.key = Keyboard::Unknown;
      break;
    case Keyboard::BackSpace:
      if (len > 0)
      {
        len--;
        buffer[len] = 0;
        changed = true;
      }
      break;
    }

    if (_uistate.keyChar >= 32 && _uistate.keyChar < 127 && len < maxLen)
    {
      buffer[len] = _uistate.keyChar;
      len++;
      buffer[len] = 0;
      changed = true;
    }
  }

  // If button is hot and active, but mouse button is not
  // down, the user must have clicked the widget; give it 
  // keyboard focus.
  if (_uistate.mouseDown == 0 && _uistate.hotItem == id && _uistate.activeItem == id)
    _uistate.keyboardItem = id;

  _uistate.lastItem = id;
  return changed;

}

//-----------------------------------------------------------------------------
bool ImGuiWindow::OnMouseMove(const Event& event)
{
  _uistate.mouseX = event.mouseMove.x;
  _uistate.mouseY = event.mouseMove.y;
  return false;
}

//-----------------------------------------------------------------------------
bool ImGuiWindow::OnMouseButtonPressed(const Event& event)
{
  if (event.mouseButton.button == Mouse::Left)
    _uistate.mouseDown |= ButtonMask::Left;
  return false;
}

//-----------------------------------------------------------------------------
bool ImGuiWindow::OnKeyPressed(const Event& event)
{
  _uistate.key = event.key.code;
  _uistate.keyMod = (event.key.shift ? ModifierMask::Shift : 0)
    | (event.key.alt ? ModifierMask::Alt : 0)
    | (event.key.control ? ModifierMask::Control : 0);
  return true;
}

//-----------------------------------------------------------------------------
bool ImGuiWindow::OnKeyReleased(const Event& event)
{
  _uistate.key = Keyboard::Unknown;
  _uistate.keyMod = (event.key.shift ? ModifierMask::Shift : 0)
    | (event.key.alt ? ModifierMask::Alt : 0)
    | (event.key.control ? ModifierMask::Control : 0);
  return true;
}

//-----------------------------------------------------------------------------
bool ImGuiWindow::OnTextEntered(const Event& event)
{
  // just accept ASCII keys
  if ((event.text.unicode & 0xff80) == 0)
    _uistate.keyChar = event.text.unicode & 0x7f;
  return true;
}

//-----------------------------------------------------------------------------
bool ImGuiWindow::OnMouseButtonReleased(const Event& event)
{
  if (event.mouseButton.button == Mouse::Left)
    _uistate.mouseDown &= ~ButtonMask::Left;
  return false;
}
