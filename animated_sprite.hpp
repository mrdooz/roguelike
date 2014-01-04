#pragma once
#include "hotload_sprite.hpp"

namespace rogue
{
  enum class Heading
  {
    South,
    SouthEast,
    East,
    NorthEast,
    North,
    NorthWest,
    West,
    SouthWest,
  };

  class RotatedSprite : public sf::Drawable
  {
    public:
    void Init(
        const TextureHandle& texture, float scale,
        const Rect& south, const Rect& east, const Rect& north, const Rect& west);

    void Init(
        const TextureHandle& texture, float scale,
        const Rect& south, const Rect& southEast, const Rect& east, const Rect& northEast,
        const Rect& north, const Rect& northWest, const Rect& west, const Rect& southWest);

    void SetColor(const Color& color);
    void SetHeading(Heading heading);
    void SetPosition(const Vector2f& pos);

    virtual void draw(RenderTarget& target, sf::RenderStates states) const;
  private:
    Rect _textureRects[8];
    mutable HotloadSprite _sprite;
  };
}
