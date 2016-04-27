#ifndef __core_CEVENTLISTENER_HPP__
#define __core_CEVENTLISTENER_HPP__

#include <functional>

namespace core
{
  class CEventListener
  {
    public:
    CEventListener();
    
    public:
    CEventListener& on(const char* event, std::function<void(void)>);
  };
}

#endif // __core_CEVENTLISTENER_HPP__
