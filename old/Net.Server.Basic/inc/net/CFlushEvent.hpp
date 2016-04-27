#ifndef __net_cflushevent_hpp__
#define __net_cflushevent_hpp__

#include <core/CEvent.hpp>
#include <net.hpp>
#include <net/CConnection.hpp>

namespace net
{
  class CFlushEvent : public core::CEvent
  {
    public:
    CConnection& mConnection;
    
    public:
    CFlushEvent(CConnection& conn) : core::CEvent("flush"), mConnection(conn)
    {
    
    }
  };
}

#endif // __net_cflushevent_hpp__
