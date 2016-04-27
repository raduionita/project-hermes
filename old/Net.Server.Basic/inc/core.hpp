#ifndef __core_hpp__
#define __core_hpp__

#ifndef _DEBUG_
  //#define _DEBUG_
#endif // _DEBUG_

#undef ERROR

#include <string>
#include <ctime>

namespace core
{
  using ushort = unsigned short;
  using uint   = unsigned int;
  using ulong  = unsigned long;
  using byte   = char;
  using ubyte  = unsigned char;
  
  using label_t = std::string;

  void noop(void);
  
  float time();
}

#endif //__core_hpp__
