#pragma once

// some code taken from various sources on
// https://github.com/LaurentGomila/SFML/wiki

// probably hacked to better fit my coding style

namespace sf
{

  class RoundedRectangleShape : public Shape
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
    virtual sf::Vector2f getPoint(unsigned int index) const;

  private:
    Vector2f _size;
    float _topLeftRadius;
    float _topRightRadius;
    float _bottomRightRadius;
    float _bottomLeftRadius;
    int _cornerPoints;
  };

  //-----------------------------------------------------------------------------
  class SFML_GRAPHICS_API LineShape : public Shape
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
}
