#include "animated_sprite.hpp"
#include "utils.hpp"

using namespace rogue;

void RotatedSprite::Init(const Texture& texture, float scale, const Rect& right, const Rect& down, const Rect& left, const Rect& up)
{
  _sprite.setTexture(texture);
  _sprite.setScale(scale, scale);
  _textureRects[(int)Heading::Right] = right;
  _textureRects[(int)Heading::Down] = down;
  _textureRects[(int)Heading::Left] = left;
  _textureRects[(int)Heading::Up] = up;
}

void RotatedSprite::Draw(const Pos& pos, Heading rot, sf::Color color, RenderTarget& rt)
{
  _sprite.setPosition(VectorCast<float>(pos));
  _sprite.setColor(color);
  _sprite.setTextureRect(_textureRects[(int)rot]);
  rt.draw(_sprite);
}
