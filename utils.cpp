#include "utils.hpp"

namespace rogue
{
#ifdef _WIN32
  bool fileExists(const char *filename)
  {
    struct _stat status;
    return _access(filename, 0) == 0 && _stat(filename, &status) == 0 && (status.st_mode & _S_IFREG);
  }
#else
  bool fileExists(const char *filename)
  {
    struct stat status;
    return access(filename, 0) == 0 && stat(filename, &status) == 0 && (status.st_mode & S_IFREG);
  }
#endif

#ifdef _WIN32
  string toString(char const * const format, ... ) 
  {
    va_list args;
    va_start(args, format);

    const int len = _vscprintf(format, args) + 1;

    char* buf = (char*)_alloca(len);
    vsprintf_s(buf, len, format, args);
    va_end(args);

    return string(buf);
  }

  //-----------------------------------------------------------------------------
  string ToString(const char* fmt, va_list args)
  {
    const int len = _vscprintf(fmt, args) + 1;

    char* buf = (char*)_alloca(len);
    vsprintf_s(buf, len, fmt, args);
    va_end(args);

    return string(buf);
  }

#else
  string toString(char const * const format, ... ) 
  {
    va_list args;
    va_start(args, format);

    va_list argcopy;
    va_copy(argcopy, args);
    int len = vsnprintf(nullptr, 0, format, argcopy) + 1;
    va_end(argcopy);

    char* buf = (char*)alloca(len);
    vsprintf(buf, format, args);
    va_end(args);

    return string(buf);
  }

  //-----------------------------------------------------------------------------
  string ToString(const char* fmt, va_list args)
  {
    va_list argcopy;
    va_copy(argcopy, args);
    int len = vsnprintf(nullptr, 0, fmt, argcopy) + 1;
    va_end(argcopy);

    char* buf = (char*)alloca(len);
    vsprintf(buf, fmt, args);
    va_end(args);

    return string(buf);
  }

#endif

  //-----------------------------------------------------------------------------
  float randf(float a, float b)
  {
    float t = (float)rand() / RAND_MAX;
    return lerp(a, b, t);
  }

  float gaussianRand(float mean, float variance) {
    // Generate a gaussian from the sum of uniformly distributed random numbers
    // (Central Limit Theorem)
    double sum = 0;
    const int numIters = 10;
    for (int i = 0; i < numIters; ++i) {
      sum += randf(-variance, variance);
    }
    return (float)(mean + sum / numIters);
  }

  //-----------------------------------------------------------------------------
  bool ArrowKeyToOffset(sf::Keyboard::Key code, Pos *ofs)
  {
    switch (code)
    {
    case sf::Keyboard::Left:  *ofs = Pos(-1,0); return true;
    case sf::Keyboard::Right: *ofs = Pos(+1,0); return true;
    case sf::Keyboard::Up:    *ofs = Pos(0,-1); return true;
    case sf::Keyboard::Down:  *ofs = Pos(0,+1); return true;
    }
    return false;
  }

  //-----------------------------------------------------------------------------
  sf::Vertex MakeVertex(int x, int y, sf::Color color)
  {
    return sf::Vertex(sf::Vector2f((float)x, (float)y), color);
  }

  //-----------------------------------------------------------------------------
  Rect operator+(const Rect& org, const Pos& ofs)
  {
    return Rect(Pos(org.left + ofs.x, org.top + ofs.y), Pos(org.width, org.height));
  }

  //-----------------------------------------------------------------------------
  Rect operator*(float s, const Rect& r)
  {
    return Rect(r.left, r.top, s * r.width, s * r.height);
  }

  //-----------------------------------------------------------------------------
  Pos operator*(float s, const Pos& a)
  {
    return Pos((int)(s * a.x), (int)(s * a.y));
  }

  //-----------------------------------------------------------------------------
  float Dist(const Pos& a, const Pos& b)
  {
    float dx = (float)(a.x - b.x);
    float dy = (float)(a.y - b.y);
    return sqrtf(dx*dx+dy*dy);
  }

  //-----------------------------------------------------------------------------
  int MDist(const Pos& a, const Pos& b)
  {
    int dx = b.x - a.x;
    int dy = b.y - a.y;

    if (dx == 0 && dy == 0)
      return 0;

    return IntAbs(dx) + IntAbs(dy);
  }

  //-----------------------------------------------------------------------------
  bool LoadFile(const char* filename, vector<char>* buf)
  {
    FILE* f = fopen(filename, "rb");
    if (!f)
      return false;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf->resize(size);
    size_t bytesRead = fread(buf->data(), 1, size, f);
    if (bytesRead != size)
    {
      fclose(f);
      return false;
    }

    return true;
  }

  //-----------------------------------------------------------------------------
  bool LoadFile(const char* filename, string* str)
  {
    vector<char> buf;
    if (!LoadFile(filename, &buf))
      return false;

    str->assign(buf.data(), buf.size());
    return true;
  }

  //-----------------------------------------------------------------------------
  float GaussianRand(float mean, float variance)
  {
    // Generate a gaussian from the sum of uniformly distributed random numbers
    // (Central Limit Theorem)
    double sum = 0;
    const int numIters = 10;
    for (int i = 0; i < numIters; ++i)
    {
      sum += randf(-variance, variance);
    }
    return (float)(mean + sum / numIters);
  }

  //-----------------------------------------------------------------------------
  void Line(int x0, int y0, int x1, int y1, vector<Pos>* line)
  {
    line->clear();

    // Bresenham between the points
    int dx = abs(x1-x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1-y0);
    int sy = y0 < y1 ? 1 : -1;

    if (dx > dy)
    {
      int ofs = 0;
      int threshold = dx;
      while (true)
      {
        line->push_back(Pos(x0,y0));
        if (x0 == x1)
          break;

        ofs += 2 * dy;
        if (ofs >= threshold)
        {
          y0 += sy;
          threshold += 2 * dx;
        }
        x0 += sx;
      }
    }
    else
    {
      int ofs = 0;
      int threshold = dy;
      while (true)
      {
        line->push_back(Pos(x0,y0));
        if (y0 == y1)
          break;

        ofs += 2 * dx;
        if (ofs >= threshold)
        {
          x0 += sx;
          threshold += 2 * dy;
        }
        y0 += sy;
      }
    }
  }

}

