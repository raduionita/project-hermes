#ifndef __core__
#define __core__

#include <string>

namespace core
{
  using ushort = unsigned short;
  using uint   = unsigned int;
  using ulong  = unsigned long;
  using byte   = char;
  using ubyte  = unsigned char;
  
  using label_t = std::string;
  
  inline void noop(void) { }
  
  void inline tolower(std::string& src)
  {
    for(size_t i = 0; i < src.size(); ++i)
      src[i] = src[i] > 0x40 && src[i] < 0x5B ? src[i] | 0x60 : src[i];
  }
}

/*
namespace std
{
  struct fgcolor
  {
    color c;
    fgcolor(std::color c) : c(c)
    {
      
    }
  };
  
  struct bgcolor
  {
    color c;
    bgcolor(std::color c) : c(c)
    {
    
    }
  };

  std::ostream& operator <<(std::ostream& out, const struct fgcolor& mod)
  {
    return out << "\033[" << mod.c << "m";
  }
  
  std::ostream& operator <<(std::ostream& out, const struct bgcolor& mod)
  {
    return out << "\033[" << (mod.c + 10) << "m";
  }
}
*/


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
