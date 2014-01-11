#include "standard_cursor.hpp"
#include "error.hpp"

using namespace sf;

#ifdef SFML_SYSTEM_WINDOWS

void StandardCursor::Set(Type t)
{
  switch(t)
  {
  case Type::Wait:
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    return;

  case Type::Text:
    SetCursor(LoadCursor(NULL, IDC_IBEAM));
    return;

  case Type::Normal:
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    return;

  case Type::SizeAll:
    SetCursor(LoadCursor(NULL, IDC_SIZEALL));
    return;

  case Type::SizeNESW:
    SetCursor(LoadCursor(NULL, IDC_SIZENESW));
    return;

  case Type::SizeNS:
    SetCursor(LoadCursor(NULL, IDC_SIZENS));
    return;

  case Type::SizeNWSE:
    SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
    return;

  case Type::SizeWE:
    SetCursor(LoadCursor(NULL, IDC_SIZEWE));
    return;

  default:
    LOG_WARN("Unknown cursor type");
    return;

    //For more cursor options on Windows go here:
    // http://msdn.microsoft.com/en-us/library/ms648391%28v=vs.85%29.aspx
  }
}

#elif defined(SFML_SYSTEM_LINUX)

sf::StandardCursor::StandardCursor(const sf::StandardCursor::TYPE t)
{
  display = XOpenDisplay(NULL);

  switch(t)
  {
  case sf::StandardCursor::WAIT:
    Cursor = XCreateFontCursor(display, XC_watch);
    break;
  case sf::StandardCursor::HAND:
    Cursor = XCreateFontCursor(display, XC_hand1);
    break;
  case sf::StandardCursor::NORMAL:
    Cursor = XCreateFontCursor(display, XC_left_ptr);
    break;
  case sf::StandardCursor::TEXT:
    Cursor = XCreateFontCursor(display, XC_xterm);
    break;
    // For more cursor options on Linux go here:
    // http://www.tronche.com/gui/x/xlib/appendix/b/
  }
}

void sf::StandardCursor::set(const sf::WindowHandle& aWindowHandle) const
{
  XDefineCursor(display, aWindowHandle, Cursor);
  XFlush(display);
}

sf::StandardCursor::~StandardCursor()
{
  XFreeCursor(display, Cursor);
  delete display;
  display = NULL;
}

#else
void StandardCursor::Set(Type t)
{

}
#endif