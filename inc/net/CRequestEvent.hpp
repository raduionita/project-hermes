#ifndef __net_crequestevent_hpp__
#define __net_crequestevent_hpp__

#include <core/CEvent.hpp>
#include <net.hpp>
#include <net/CRequest.hpp>

namespace net
{
  class CRequestEvent : public core::CEvent
  {
    public:
    CRequest& mRequest;
    
    public:
    CRequestEvent(CRequest& req) : core::CEvent("request"), mRequest(req)
    {
      
    }
  };
}

#endif // __net_crequestevent_hpp__
