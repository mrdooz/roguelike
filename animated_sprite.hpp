#pragma once

namespace rogue
{
  enum class Heading
  {
    Right,
    Down,
    Left,
    Up
  };

  struct RotatedSprite
  {
    void Init(const Texture& texture, float scale, const Rect& right, const Rect& down, const Rect& left, const Rect& up);
    void Draw(const Pos& pos, Heading rot, sf::Color color, RenderTarget& rt);

    Rect _textureRects[4];
    Sprite _sprite;
  };
}
