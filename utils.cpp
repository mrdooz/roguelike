#include "stdafx.h"
#include "utils.hpp"

using namespace std;

bool fileExists(const char *filename)
{
  struct _stat status;
  return _access(filename, 0) == 0 && _stat(filename, &status) == 0 && (status.st_mode & _S_IFREG);
}


string toString(char const * const format, ... ) 
{
  va_list arg;
  va_start(arg, format);

  const int len = _vscprintf(format, arg) + 1;

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, format, arg);
  va_end(arg);

  return string(buf);
}

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
