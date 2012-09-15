#pragma once

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
