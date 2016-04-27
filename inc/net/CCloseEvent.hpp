#ifndef __net_ccloseevent_hpp__
#define __net_ccloseevent_hpp__

#include <core/CEvent.hpp>
#include <net.hpp>

namespace net
{
  class CCloseEvent : public core::CEvent
  {
    public:
    
    public:
    CCloseEvent() : core::CEvent("close")
    {
      
    }
  };
}

#endif // __net_ccloseevent_hpp__
