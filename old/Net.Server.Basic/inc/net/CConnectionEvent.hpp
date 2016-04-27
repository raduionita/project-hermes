#ifndef __net_cconnectionevent_hpp__
#define __net_cconnectionevent_hpp__

#include <core/CEvent.hpp>
#include <net.hpp>
#include <net/CConnection.hpp>

namespace net
{
  class CConnectionEvent : public core::CEvent
  {
    public:
    CConnection& mConnection;
    
    public:
    CConnectionEvent(CConnection& conn) : core::CEvent("connection"), mConnection(conn)
    {
      // mothing
    }
  };
}
#endif // __net_cconnectionevent_hpp__
