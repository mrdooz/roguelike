#include "virtual_window.hpp"
#include "utils.hpp"

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
{
  _texture.create((int)size.x, (int)size.y);
  _sprite.setTexture(_texture.getTexture());
  _sprite.setPosition(pos);
}

//-----------------------------------------------------------------------------
void VirtualWindow::SetPosition(const Vector2f& pos)
{
  _pos = pos;
  _sprite.setPosition(pos);
}
