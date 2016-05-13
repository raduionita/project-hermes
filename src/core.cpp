#include <core.hpp>

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

namespace core
{
  void noop(void)
  {
    // do nothing..
  }
}
