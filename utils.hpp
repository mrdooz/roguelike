#pragma once

#define ELEMS_IN_ARRAY(x) sizeof(x) / sizeof((x)[0])

template <class T>
T exch_null(T &t)
{
  T tmp = t;
  t = nullptr;
  return tmp;
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

std::string toString(char const * const format, ...);

float lerp(float a, float b, float v);
float randf(float a, float b);
float gaussianRand(float mean, float variance);
