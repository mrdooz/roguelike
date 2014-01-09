#include "shapes.hpp"

using namespace sf;
using namespace rogue;

//-----------------------------------------------------------------------------
RoundedRectangleShape::RoundedRectangleShape(
    const Vector2f& size,
    float topLeftRadius,
    float topRightRadius,
    float bottomRightRadius,
    float bottomLeftRadius,
    int cornerPoints)
  : _size(size)
  , _topLeftRadius(topLeftRadius)
  , _topRightRadius(topRightRadius)
  , _bottomRightRadius(bottomRightRadius)
  , _bottomLeftRadius(bottomLeftRadius)
  , _cornerPoints(max(1, cornerPoints))
{
  update();
}

//-----------------------------------------------------------------------------
unsigned int RoundedRectangleShape::getPointCount() const
{
  return _cornerPoints * 4;
}

//-----------------------------------------------------------------------------
Vector2f RoundedRectangleShape::getPoint(unsigned int index) const
{
  static const float PI = 3.141592654f;

  Vector2f center;
  int idx = index / _cornerPoints;
  float r;
  switch (idx)
  {
    // top right
  case 0:
    r = _topRightRadius;
    center.x = _size.x - r;
    center.y = r;
    break;

    // top left
  case 1:
    r = _topLeftRadius;
    center.x = r;
    center.y = r;
    break;

    // bottom left
  case 2:
    r = _bottomLeftRadius;
    center.x = r;
    center.y = _size.y - r;
    break;

    // bottom right
  case 3:
    r = _bottomRightRadius;
    center.x = _size.x - r;
    center.y = _size.y - r;
    break;
  }

  // calc starting angle
  float start = idx * PI / 2;
  float angle = PI / 2 * (index - idx * _cornerPoints) / _cornerPoints;
  return center + Vector2f(r * cos(start + angle), -r * sin(start + angle));
}

//-----------------------------------------------------------------------------
LineShape::LineShape(const Vector2f& point1, const Vector2f& point2, float thickness)
  : m_direction(point2 - point1)
  , m_thickness(thickness)
{
  setPosition(point1);
  update();
}

//-----------------------------------------------------------------------------
void LineShape::setThickness(float thickness)
{
  m_thickness = thickness;
  update();
}

//-----------------------------------------------------------------------------
float LineShape::getThickness() const
{
  return m_thickness;
}

//-----------------------------------------------------------------------------
float LineShape::getLength() const
{
  return std::sqrt(m_direction.x*m_direction.x+m_direction.y*m_direction.y);
}


//-----------------------------------------------------------------------------
unsigned int LineShape::getPointCount() const
{
  return 4;
}

//-----------------------------------------------------------------------------
Vector2f LineShape::getPoint(unsigned int index) const
{
  Vector2f unitDirection = m_direction/getLength();
  Vector2f unitPerpendicular(-unitDirection.y,unitDirection.x);

  Vector2f offset = (m_thickness/2.f)*unitPerpendicular;

  switch (index)
  {
  default:
  case 0: return offset;
  case 1: return (m_direction + offset);
  case 2: return (m_direction - offset);
  case 3: return (-offset);
  }
}