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



  float lerp(float a, float b, float v) {
    return (1-v) * a + v * b;
  }

  float randf(float a, float b) {
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
  float Dist(const Pos& a, const Pos& b)
  {
    float dx = (float)(a.x - b.x);
    float dy = (float)(a.y - b.y);
    return sqrtf(dx*dx+dy*dy);
  }
}

