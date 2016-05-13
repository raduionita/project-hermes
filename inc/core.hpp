#ifndef __core__
#define __core__

#include <string>
#include <ostream>

namespace core
{
  using ushort = unsigned short;
  using uint   = unsigned int;
  using ulong  = unsigned long;
  using byte   = char;
  using ubyte  = unsigned char;
  
  using label_t = std::string;
  
  void noop(void);
}

/*
namespace std
{
#ifdef _WIN32_WINNT
    enum color { DARKBLUE = 1, DARKGREEN, DARKTEAL, DARKRED, DARKPINK, DARKYELLOW, GRAY, DARKGRAY, BLUE, GREEN, TEAL, RED, PINK, YELLOW, WHITE };
#else
    enum code { DARKBLUE = 1, DARKGREEN, DARKTEAL, DARKRED, DARKPINK, DARKYELLOW, GRAY, DARKGRAY, BLUE = 34, GREEN = 32, TEAL, RED = 31, PINK, YELLOW, WHITE };
#endif // _WIN32_WINNT
  
  void fgcolor(std::ostream& out, color col);
  
  void bgcolor(std::ostream& out, color col);

  std::ostream& operator <<(std::ostream& out, void(*modifier)(std::ostream&, color));
}
*/

#endif // __core__
