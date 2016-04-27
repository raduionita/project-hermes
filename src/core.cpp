#include <core.hpp>

namespace core
{
  void noop(void) { }
  
  float time()
  {
    return clock()/CLOCKS_PER_SEC;
  }
}
