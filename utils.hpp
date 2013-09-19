#pragma once

#define ELEMS_IN_ARRAY(x) sizeof(x) / sizeof((x)[0])

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
  void seq_delete(T *seq) {
    for (auto it = begin(*seq); it != end(*seq); ++it)
      delete *it;
    seq->clear();
  }

  template<class T> 
  void assoc_delete(T* t) {
    for (T::iterator it = t->begin(); it != t->end(); ++it)
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

  float lerp(float a, float b, float v);
  float randf(float a, float b);
  float gaussianRand(float mean, float variance);

  template <class T>
  bool contains(const T &cont, const typename T::value_type &key) {
    return cont.find(key) != end(cont);
  }
}
