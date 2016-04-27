#ifndef __net_http_cevent_hpp__
#define __net_http_cevent_hpp__

namespace net
{
  namespace http
  {
    class CEvent
    {
      typedef const std::string label_t;
      
      public:
      virtual ~CEvent() { } // w/o virtual is not polymorphic
    };
  }
}

#endif // __net_http_cevent_hpp__
