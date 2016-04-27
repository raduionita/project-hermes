#ifndef __net_cerrorevent_hpp__
#define __net_cerrorevent_hpp__

#include <core/CEvent.hpp>
#include <net.hpp>
#include <net/CError.hpp>

namespace net
{
  class CErrorEvent : public core::CEvent
  {
    public:
    CError mError;
    
    public:
    CErrorEvent(const CError& err) : core::CEvent("error"), mError(err)
    {
      
    }
  };
}

#endif // __net_cerrorevent_hpp__
