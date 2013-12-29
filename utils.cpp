#include "utils.hpp"

using namespace rogue;

#ifdef _WIN32
bool rogue::fileExists(const char *filename)
{
  struct _stat status;
  return _access(filename, 0) == 0 && _stat(filename, &status) == 0 && (status.st_mode & _S_IFREG);
}
#else
bool rogue::fileExists(const char *filename)
{
  struct stat status;
  return access(filename, 0) == 0 && stat(filename, &status) == 0 && (status.st_mode & S_IFREG);
}
#endif

#ifdef _WIN32
string rogue::toString(char const * const format, ... ) 
{
  va_list arg;
  va_start(arg, format);

  const int len = _vscprintf(format, arg) + 1;

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, format, arg);
  va_end(arg);

  return string(buf);
}
#else
string rogue::toString(char const * const format, ... ) 
{
  return string();
}
#endif


float rogue::lerp(float a, float b, float v) {
  return (1-v) * a + v * b;
}

float rogue::randf(float a, float b) {
  float t = (float)rand() / RAND_MAX;
  return lerp(a, b, t);
}

float rogue::gaussianRand(float mean, float variance) {
  // Generate a gaussian from the sum of uniformly distributed random numbers
  // (Central Limit Theorem)
  double sum = 0;
  const int numIters = 10;
  for (int i = 0; i < numIters; ++i) {
    sum += randf(-variance, variance);
  }
  return (float)(mean + sum / numIters);
}
