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

  struct Animation
  {
    enum class Id
    {
      None,
      Blood,
      ArcaneBlast
    };

    Animation(Id id, const Texture& texture, time_duration duration);
    Id _id;
    Texture _texture;
    time_duration _duration;
    vector<IntRect> _textureRects;
    bool _looping;
  };

  struct AnimationInstance
  {
    AnimationInstance();
    Sprite _sprite;
    Animation* _animation;
    ptime _startTime;
    ptime _endTime;
    time_duration _duration;
    Pos _startPos;
    Pos _endPos;
  };


  class RotatedSprite : public sf::Drawable
  {
    public:
    void Init(
        const Texture& texture, float scale,
        const Rect& south, const Rect& east, const Rect& north, const Rect& west);

    void Init(
        const Texture& texture, float scale,
        const Rect& south, const Rect& southEast, const Rect& east, const Rect& northEast,
        const Rect& north, const Rect& northWest, const Rect& west, const Rect& southWest);

    void SetColor(const Color& color);
    void SetHeading(Heading heading);
    void SetPosition(const Vector2f& pos);

    virtual void draw(RenderTarget& target, sf::RenderStates states) const;
  private:
    Rect _textureRects[8];
    Sprite _sprite;
  };
}
