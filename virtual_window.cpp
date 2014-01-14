#include "virtual_window.hpp"
#include "utils.hpp"
#include "sfml_helpers.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
VirtualWindow::VirtualWindow(
    const string& title,
    const Vector2f& pos,
    const Vector2f& size)
  : _windowManager(nullptr)
  , _title(title)
  , _pos(pos)
  , _size(size)
  , _depth(0)
  , _borderWidth(4)
  , _titleBarHeight(10)
  , _focus(false)
  , _moving(false)
  , _resizing(false)
{
  _texture.create((int)size.x, (int)size.y);
  _sprite.setTexture(_texture.getTexture());
  _sprite.setPosition(pos);
}

//-----------------------------------------------------------------------------
bool VirtualWindow::Init()
{
  if (!_font.loadFromFile("gfx/wscsnrg.ttf"))
    return false;

  return true;
}

//-----------------------------------------------------------------------------
void VirtualWindow::SetSize(const Vector2f& size)
{
  _size = size;
  _texture.create((int)size.x, (int)size.y);
  _sprite.setTexture(_texture.getTexture(), true);
}

//-----------------------------------------------------------------------------
void VirtualWindow::SetPosition(const Vector2f& pos)
{
  _pos = pos;
  _sprite.setPosition(pos);
}

//-----------------------------------------------------------------------------
void VirtualWindow::DrawBorder(RenderWindow* window)
{
  float w = (float)_borderWidth;
  RoundedRectangleShape rect(_size + Vector2f(2*w, w+10), 10, 10, 0, 0, 40);
  rect.setPosition(_pos - Vector2f(w, 10));
  rect.setFillColor(_focus ? Color(50, 50, 50) : Color(30, 30, 30));

  // center the text (taking border into account)
  Text text(_title, _font, 8);
  float tw = text.getLocalBounds().width;
  text.setPosition(_pos + Vector2f(w + (_size.x - tw) / 2, -10));
  text.setColor(Color::White);

  // Draw to the underlying render target
  window->draw(rect);
  window->draw(text);
}

//-----------------------------------------------------------------------------
bool VirtualWindow::PointInside(const Vector2f& pos, bool includeBorder)
{
  float w = includeBorder ? (float)_borderWidth : 0;
  float h = includeBorder ? (float)_titleBarHeight : 0;

  FloatRect rect(_pos - Vector2f(w, h),
    Vector2f(2*w + _size.x, h + _size.y + w));

  return rect.contains(pos);
}

//-----------------------------------------------------------------------------
u32 VirtualWindow::PointInsideBorder(const Vector2f& pos)
{
  //enum { LeftBorder = 1, RightBorder = 2, TopBorder = 4, BottomBorder = 8};

  float w = (float)_borderWidth;
  float h = (float)_titleBarHeight;

  u32 mask = 0;

  FloatRect rectLeft(_pos - Vector2f(w, h), Vector2f(w, h+_size.y+w));
  if (rectLeft.contains(pos))
    mask |= LeftBorder;

  FloatRect rectRight(_pos + Vector2f(_size.x, -h), Vector2f(w, h+_size.y+w));
  if (rectRight.contains(pos))
    mask |= RightBorder;

  FloatRect rectTop(_pos - Vector2f(w, h), Vector2f(2*w + _size.x, h));
  if (rectTop.contains(pos))
    mask |= TopBorder;

  FloatRect rectTopOuter(_pos - Vector2f(w, h), Vector2f(2*w + _size.x, w));
  if (rectTopOuter.contains(pos))
    mask |= TopBorderOuter;

  FloatRect rectBottom(_pos + Vector2f(-w, _size.y), Vector2f(2*w + _size.x, w));
  if (rectBottom.contains(pos))
    mask |= BottomBorder;

  return mask;
}
