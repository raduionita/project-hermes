#ifndef __net_http_crequestevent_hpp__
#define __net_http_crequestevent_hpp__

#include <net/http/CEvent.hpp>
#include <net/http/CRequest.hpp>
#include <net/http/CResponse.hpp>

namespace net
{
  namespace http
  {
    class CRequestEvent : public CEvent
    {
      public:
      CRequest  mRequest;
      CResponse mResponse;
    };
  }
}

#endif // __net_http_crequestevent_hpp__
