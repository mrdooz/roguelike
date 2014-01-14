#pragma once

// some code taken from various sources:
// https://github.com/LaurentGomila/SFML/wiki
// http://www.cs.rit.edu/~ncs/color/t_convert.html

// probably hacked to better fit my coding style

namespace rogue
{
  class RoundedRectangleShape : public sf::Shape
  {
  public:
    RoundedRectangleShape(
      const Vector2f& size,
      float topLeftRadius,
      float topRightRadius,
      float bottomRightRadius,
      float bottomLeftRadius,
      int cornerPoints);

    virtual unsigned int getPointCount() const;
    virtual Vector2f getPoint(unsigned int index) const;

  private:
    Vector2f _size;
    float _topLeftRadius;
    float _topRightRadius;
    float _bottomRightRadius;
    float _bottomLeftRadius;
    int _cornerPoints;
  };

  //-----------------------------------------------------------------------------
  class LineShape : public sf::Shape
  {
  public:

    LineShape(const Vector2f& point1, const Vector2f& point2, float thickness = 2);

    void setThickness(float thickness);
    float getThickness() const;
    float getLength() const;

    virtual unsigned int getPointCount() const;
    virtual Vector2f getPoint(unsigned int index) const;

  private:
    Vector2f m_direction; ///< Direction of the line
    float m_thickness;    ///< Thickness of the line
  };

  //-----------------------------------------------------------------------------
  struct HsvColor
  {
    HsvColor() : h(0), s(0), v(0) {}
    HsvColor(float h, float s, float v);
    operator Color() const;
    float h, s, v;
  };
  //-----------------------------------------------------------------------------
  Color ColorFromHsv(int hue, float sat, float val);
  HsvColor HsvFromColor(const Color& color);

  inline u32 ColorToU32(const Color& color)
  {
    return (color.r << 0) | (color.g << 8) | (color.b << 16) | (color.a << 24);
  }

  inline Color ColorFromU32(u32 col)
  {
    return Color(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff, (col >> 24) & 0xff);
  }
}
