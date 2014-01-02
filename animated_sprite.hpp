#pragma once

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

  enum class AnimatedSpriteId
  {

  };

  class RotatedSprite : public sf::Drawable
  {
    public:

    void Init(const Texture& texture, float scale, const Rect& south, const Rect& east, const Rect& north, const Rect& west);
    void Init(
        const Texture& texture, float scale,
        const Rect& south, const Rect& southEast, const Rect& east, const Rect& northEast,
        const Rect& north, const Rect& northWest, const Rect& west, const Rect& southWest);

    void SetColor(const Color& color);
    void SetHeading(Heading heading);
    void SetPosition(const Vector2f& pos);

    virtual void draw(RenderTarget& target, sf::RenderStates states) const;
//    void Draw(const Pos& pos, Heading rot, sf::Color color, RenderTarget& rt);
  private:
    Rect _textureRects[8];
    Sprite _sprite;
  };
}
