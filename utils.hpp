#pragma once

#define ELEMS_IN_ARRAY(x) sizeof(x) / sizeof((x)[0])

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);   \
  void operator=(const TypeName&)

namespace rogue
{
  template <class T>
  T exch_null(T &t)
  {
    T tmp = t;
    t = nullptr;
    return tmp;
  }

  template<class T> 
  void SeqDelete(T *seq) {
    for (auto it = begin(*seq); it != end(*seq); ++it)
      delete *it;
    seq->clear();
  }

  template<class T> 
  void AssocDelete(T* t) {
    for (typename T::iterator it = t->begin(); it != t->end(); ++it)
      delete it->second;
    t->clear();
  }

  bool fileExists(const char *filename);

  inline uint32 setBit(uint32 value, int bit_num) {
    return (value | 1 << bit_num);
  }

  inline bool isBitSet(uint32 value, int bit_num) {
    return !!(value & (1 << bit_num));
  }

  inline uint32 clearBit(uint32 value, int bit_num) {
    return value & ~(1 << bit_num);
  }

  string toString(char const * const format, ...);
  string ToString(const char* fmt, va_list args);

  Rect operator+(const Rect& org, const Pos& ofs);
  Pos operator*(float s, const Pos& a);
  Rect operator*(float s, const Rect& r);

  template <typename T>
  T lerp(T a, T b, float v)
  {
    return (1-v) * a + v * b;
  }

  float randf(float a, float b);
  float gaussianRand(float mean, float variance);

  template <class T>
  bool contains(const T &cont, const typename T::value_type &key) {
    return cont.find(key) != end(cont);
  }

  bool ArrowKeyToOffset(sf::Keyboard::Key code, Pos *ofs);

  sf::Vertex MakeVertex(int x, int y, sf::Color color = sf::Color::White);

  template <typename To, typename From>
  sf::Vector2<To> VectorCast(const sf::Vector2<From>& src)
  {
    return sf::Vector2<To>((To)src.x, (To)src.y);
  }


  inline int IntAbs(int a)
  {
    return a > 0 ? a : -a;
  }

  float Dist(const Pos& a, const Pos& b);

  // Manhattan distance
  int MDist(const Pos& a, const Pos& b);

  bool LoadFile(const char* filename, vector<char>* buf);
  bool LoadFile(const char* filename, string* str);

  template <typename T>
  T Clamp(T v, T minValue, T maxValue)
  {
    return max(minValue, min(maxValue, v));
  }

  template <typename T>
  T min3(T a, T b, T c)
  {
    return min(c, min(a, b));
  }

  template <typename T>
  T max3(T a, T b, T c)
  {
    return max(c, max(a, b));
  }

  template <typename T, typename U>
  T Lerp(T a, T b, U v)
  {
    return (1-v) * a + v * b;
  }

  template <typename T>
  T Randf(T a, T b)
  {
    float t = (float)rand() / RAND_MAX;
    return Lerp(a, b, t);
  }

  float GaussianRand(float mean, float variance);

  void Line(int x0, int y0, int x1, int y1, vector<Pos>* line);

  // Macro for creating "local" names
#define GEN_NAME2(prefix, line) prefix##line
#define GEN_NAME(prefix, line) GEN_NAME2(prefix, line)

}
