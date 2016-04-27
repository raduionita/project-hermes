#ifndef __net_cmessageevent_hpp__
#define __net_cmessageevent_hpp__

#include <core/CEvent.hpp>
#include <net.hpp>
#include <net/CMessage.hpp>

namespace net
{
  class CMessageEvent : public core::CEvent
  {
    public:
    CMessage& mMessage;
    
    public:
    CMessageEvent(CMessage& message) : core::CEvent("message"), mMessage(message)
    {
    
    }
  };
}

#endif // __net_cmessageevent_hpp__
