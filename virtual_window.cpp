#include "virtual_window.hpp"
#include "utils.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
VirtualWindow::VirtualWindow(
    const string& title,
    const Vector2u& pos,
    const Vector2u& size)
  : _windowManager(nullptr)
  , _title(title)
  , _pos(pos)
  , _size(size)
  , _borderWidth(4)
  , _focus(false)
{
  _texture.create(size.x + 2 * _borderWidth, size.y + 2 * _borderWidth);
  _sprite.setTexture(_texture.getTexture());
  _sprite.setPosition(VectorCast<float>(pos));
}

//-----------------------------------------------------------------------------
