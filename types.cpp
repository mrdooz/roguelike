namespace rogue
{
  //-----------------------------------------------------------------------------
  Pos::Pos()
    : row(~0)
    , col(~0)
  {
  }

  //-----------------------------------------------------------------------------
  Pos::Pos(int r, int c)
    : row(r)
    , col(c)
  {
  }

  //-----------------------------------------------------------------------------
  bool Pos::IsValid() const
  {
    return !(row == ~0 || col == ~0);
  }

  //-----------------------------------------------------------------------------
  Pos operator+(const Pos &a, const Pos &b)
  {
    return Pos(a.row + b.row, a.col + b.col);
  }

  //-----------------------------------------------------------------------------
  Pos operator-(const Pos &a, const Pos &b)
  {
    return Pos(a.row - b.row, a.col - b.col);
  }

  //-----------------------------------------------------------------------------
  bool operator==(const Pos &a, const Pos &b)
  {
    return a.row == b.row && a.col == b.col;
  }

  //-----------------------------------------------------------------------------
  bool operator<(const Pos &a, const Pos &b)
  {
    if (a.x < b.x) return true;
    if (b.x < a.x) return false;
    if (a.y < b.y) return true;
    if (b.y < a.y) return false;
    return false;
  }

  //-----------------------------------------------------------------------------
  Rect::Rect()
    : top(~0)
    , left(~0)
    , bottom(~0)
    , right(~0)
  {
  }

  //-----------------------------------------------------------------------------
  Rect::Rect(int top, int left, int bottom, int right) 
    : top(min(top, bottom))
    , left(min(left, right))
    , bottom(max(top, bottom))
    , right(max(left, right))
  {
  }

  //-----------------------------------------------------------------------------
  Rect::Rect(const Pos& pos, int width, int height)
    : top(pos.y)
    , left(pos.x)
    , bottom(top + height)
    , right(left + width)
  {
  }

  //-----------------------------------------------------------------------------
  bool Rect::IsValid() const
  {
    return !(top == ~0 || left == ~0 || bottom == ~0 || right == ~0);
  }

  //-----------------------------------------------------------------------------
  int Rect::Width() const
  {
    return right - left;
  }

  //-----------------------------------------------------------------------------
  int Rect::Height() const
  {
    return bottom - top;
  }

  //-----------------------------------------------------------------------------
  bool Rect::PointInside(const Pos& pt) const
  {
    return pt.x >= left && pt.x < right && pt.y >= top && pt.y < bottom;
  }

}