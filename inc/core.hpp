#ifndef __core__
#define __core__

#include <string>
// #include <algorithm> // conflicts with logger

namespace core
{
  using ushort = unsigned short;
  using uint   = unsigned int;
  using ulong  = unsigned long;
  using byte   = char;
  using ubyte  = unsigned char;
  
  using label_t = std::string;
  
  inline void noop(void) { }
  
  inline void tolower(std::string& src)
  {
    for(size_t i = 0; i < src.size(); ++i)
      src[i] = src[i] > 0x40 && src[i] < 0x5B ? src[i] | 0x60 : src[i];
  }
  
   // @todo This sucks, fix it
  inline std::string basename(const std::string& path)
  {
    std::string base;
    base.reserve(path.size());
    for(size_t i = path.size() - 1, j = 0; i > 0; --i)
    {
      char ch = path[i];
      if(ch == 0x2D || ch == 0x2E || (ch >= 0x30 && ch <= 0x39) || (ch >= 0x41 && ch <= 0x5A) || ch == 0x5F || (ch >= 0x61 && ch <= 0x7A))
        base[j++] = ch;
    }
    base.shrink_to_fit();
    return base;
  }
  
/*
  // trim from start
  static inline std::string &ltrim(std::string &s) 
  {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
  }

  static inline std::string &rtrim(std::string &s) 
  {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
  }

  static inline std::string &trim(std::string &s) 
  {
    return ltrim(rtrim(s));
  }
*/
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
