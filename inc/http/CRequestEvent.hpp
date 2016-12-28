#ifndef __http_crequestevent_hpp__
#define __http_crequestevent_hpp__

#include <http.hpp>
#include <http/CRequest.hpp>
#include <core/CEvent.hpp>

#include <memory>

namespace http
{
  class CRequestEvent : public CEvent
  {
    typedef std::shared_ptr<CRequest>  request_t;

    public:
    request_t mRequest;

    public:
    CRequestEvent(request_t request) : CEvent("request"), mRequest(request)
    {
      log::debug << "http::CRequestEvent::CRequestEvent(request)" << log::endl;
    }
  };
}

#endif // __http_crequestevent_hpp__
