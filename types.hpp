#ifndef _TYPES_HPP_
#define _TYPES_HPP_

struct Pos {
  Pos() : row(~0), col(~0) {}
  Pos(int r, int c) : row(r), col(c) {}
  friend Pos operator+(const Pos &a, const Pos &b) { return Pos(a.row + b.row, a.col + b.col); }
  friend Pos operator-(const Pos &a, const Pos &b) { return Pos(a.row - b.row, a.col - b.col); }
  friend bool operator==(const Pos &a, const Pos &b) { return a.row == b.row && a.col == b.col; }
  friend bool operator<(const Pos &a, const Pos &b) {
    if (a.x < b.x) return true;
    if (b.x < a.x) return false;
    if (a.y < b.y) return true;
    if (b.y < a.y) return false;
    return false;
  }
  union {
    int row;
    int y;
  };
  union {
    int col;
    int x;
  };
};

#endif