#ifndef __core_clooper_hpp__
#define __core_clooper_hpp__

#include <functional>

namespace core
{
  class CLooper
  {
    public:
    virtual void loop(std::function<void(void)> callback)
    {
      while(true)
      {
        callback();
      }
    }
  };
}

#endif // __core_clooper_hpp__
