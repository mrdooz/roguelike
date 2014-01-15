#include "sfml_helpers.hpp"
#include "utils.hpp"

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

//-----------------------------------------------------------------------------
// hue: 0-360°; sat: 0.f-1.f; val: 0.f-1.f
Color rogue::ColorFromHsv(int hue, float sat, float val)
{
  hue %= 360;
  while(hue<0) hue += 360;

  sat = Clamp(sat, 0.f, 1.f);
  val = Clamp(val, 0.f, 1.f);

  int h = hue/60;
  float f = float(hue)/60-h;
  float p = val*(1.f-sat);
  float q = val*(1.f-sat*f);
  float t = val*(1.f-sat*(1-f));

  switch(h)
  {
  default:
  case 0:
  case 6: return Color(val*255, t*255, p*255);
  case 1: return Color(q*255, val*255, p*255);
  case 2: return Color(p*255, val*255, t*255);
  case 3: return Color(p*255, q*255, val*255);
  case 4: return Color(t*255, p*255, val*255);
  case 5: return Color(val*255, p*255, q*255);
  }
}

//-----------------------------------------------------------------------------
HsvColor rogue::HsvFromColor(const Color& color)
{
  // r,g,b values are from 0 to 1
  // h = [0,360], s = [0,1], v = [0,1]
  //		if s == 0, then h = -1 (undefined)

  HsvColor res;
  float r = color.r / 255.0f;
  float g = color.g / 255.0f;
  float b = color.b / 255.0f;

  float min, max, delta;
  min = min3( r, g, b );
  max = max3( r, g, b );
  // v
  res.v = max;
  delta = max - min;
  if( max != 0 )
  {
    // s
    res.s = delta / max;
  }
  else
  {
    // r = g = b = 0
    // s = 0, v is undefined
    res.s = 0;
    res.h = -1;
    return res;
  }

  if( r == max )
    // between yellow & magenta
    res.h = ( g - b ) / delta;
  else if( g == max )
    // between cyan & yellow
    res.h = 2 + ( b - r ) / delta;
  else
    // between magenta & cyan
    res.h = 4 + ( r - g ) / delta;

  // degrees
  res.h *= 60;
  if( res.h < 0 )
    res.h += 360;

  return res;
}

//-----------------------------------------------------------------------------
HsvColor::HsvColor(float h, float s, float v)
  : h(h), s(s), v(v)
{
}

//-----------------------------------------------------------------------------
HsvColor::operator Color() const
{
  return ColorFromHsv(h, s, v);
}
