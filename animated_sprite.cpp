#include "animated_sprite.hpp"
#include "utils.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
Animation::Animation(Id id, const Texture& texture, time_duration duration)
  : _id(id)
  , _texture(texture)
  , _duration(duration)
  , _looping(false)
{
}

//-----------------------------------------------------------------------------
AnimationInstance::AnimationInstance()
  : _animation(nullptr)
{
  _sprite.setScale(3, 3);
}


//-----------------------------------------------------------------------------
void RotatedSprite::Init(const Texture& texture, float scale, const Rect& south, const Rect& east, const Rect& north, const Rect& west)
{
  _sprite.setTexture(texture);
  _sprite.setScale(scale, scale);
  _textureRects[(int)Heading::South]  = south;
  _textureRects[(int)Heading::East]   = east;
  _textureRects[(int)Heading::North]  = north;
  _textureRects[(int)Heading::West]   = west;
}


//-----------------------------------------------------------------------------
void RotatedSprite::Init(
    const Texture& texture, float scale,
    const Rect& south, const Rect& southEast, const Rect& east, const Rect& northEast,
    const Rect& north, const Rect& northWest, const Rect& west, const Rect& southWest)
{
  _sprite.setTexture(texture);
  _sprite.setScale(scale, scale);
  _textureRects[(int)Heading::South]      = south;
  _textureRects[(int)Heading::SouthEast]  = southEast;
  _textureRects[(int)Heading::East]       = east;
  _textureRects[(int)Heading::NorthEast]  = northEast;
  _textureRects[(int)Heading::North]      = north;
  _textureRects[(int)Heading::NorthWest]  = northWest;
  _textureRects[(int)Heading::West]       = west;
  _textureRects[(int)Heading::SouthWest]  = southWest;
}

//-----------------------------------------------------------------------------
void RotatedSprite::SetColor(const Color& color)
{
  _sprite.setColor(color);
}

//-----------------------------------------------------------------------------
void RotatedSprite::SetHeading(Heading heading)
{
  _sprite.setTextureRect(_textureRects[(int)heading]);
}

//-----------------------------------------------------------------------------
void RotatedSprite::SetPosition(const Vector2f& pos)
{
  _sprite.setPosition(pos);
}

//-----------------------------------------------------------------------------
void RotatedSprite::draw(RenderTarget& target, sf::RenderStates states) const
{
  target.draw(_sprite);
}
