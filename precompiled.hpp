#ifndef _PRECOMPILED_HPP_
#define _PRECOMPILED_HPP_

#include <stdio.h>

#include <SFML/Graphics.hpp>

#ifdef _WIN32
  #include <windows.h>
  #include <direct.h>
  #include <io.h>
#endif

#include <assert.h>
#include <vector>
#include <string>

#include <sys/stat.h>
#include <string>
#include <stdint.h>

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;


#ifdef _DEBUG
#pragma comment(lib, "sfml-graphics-s-d.lib")
#pragma comment(lib, "sfml-window-s-d.lib")
#pragma comment(lib, "sfml-system-s-d.lib")
#else
#pragma comment(lib, "sfml-graphics-s.lib")
#pragma comment(lib, "sfml-window-s.lib")
#pragma comment(lib, "sfml-system-s.lib")
#endif

#endif
