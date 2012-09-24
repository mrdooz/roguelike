#ifndef _TYPES_HPP_
#define _TYPES_HPP_

struct Pos {
  Pos() : row(~0), col(~0) {}
  Pos(int r, int c) : row(r), col(c) {}
  friend Pos operator+(const Pos &a, const Pos &b) { return Pos(a.row + b.row, a.col + b.col); }
  friend Pos operator-(const Pos &a, const Pos &b) { return Pos(a.row - b.row, a.col - b.col); }
  int row, col;
};

#endif