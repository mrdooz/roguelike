#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include "roguelike.hpp"

namespace rogue
{
  //-----------------------------------------------------------------------------
  struct Pos
  {
    Pos();
    Pos(int r, int c);
    bool IsValid() const;

    union
    {
      int row;
      int y;
    };
    union
    {
      int col;
      int x;
    };
  };

  Pos operator+(const Pos &a, const Pos &b);
  Pos operator-(const Pos &a, const Pos &b);
  bool operator==(const Pos &a, const Pos &b);
  bool operator<(const Pos &a, const Pos &b);

  //-----------------------------------------------------------------------------
  struct Rect
  {
    // span is [top, left] -> (bottom, right)
    Rect();
    Rect(int top, int left, int bottom, int right);
    Rect(const Pos& pos, int width, int height);

    bool IsValid() const;
    int Width() const;
    int Height() const;

    bool PointInside(const Pos& pt) const;

    int top;
    int left;
    int bottom;
    int right;
  };
}

#endif