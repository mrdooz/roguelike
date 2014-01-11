#pragma once

// based off https://github.com/LaurentGomila/SFML/wiki/Tutorial:-Change-Cursor
// but with some semantic changes

#ifndef STANDARDCURSOR_HPP
#define STANDARDCURSOR_HPP

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#ifdef SFML_SYSTEM_WINDOWS
#include <windows.h>
#elif defined(SFML_SYSTEM_LINUX)
#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#else
//#error This OS is not yet supported by the cursor library.
#endif

namespace sf
{
  class StandardCursor
  {
  public:
    enum class Type { Wait, Text, Normal, SizeAll, SizeNESW, SizeNS, SizeNWSE, SizeWE };
    static void Set(Type t);
#if 0
    XID Cursor;
    Display* display;
#endif
  };
}

#endif // STANDARDCURSOR_HPP