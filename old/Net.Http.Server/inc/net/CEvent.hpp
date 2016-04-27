#ifndef __net_cevent_hpp__
#define __net_cevent_hpp__

#include <core/CEvent.hpp>

namespace net
{
  namespace event
  {
    core::CEventType ERROR("error");
    core::CEventType REQUEST("request");
    core::CEventType RESPONSE("response");
  }

  class CEvent : public core::CEvent
  {
    
  };
  
  class CErrorEvent : public core::CErrorEvent, CEvent
  {
    public:
    core::CEventType& getType() const
    {
      return event::ERROR;
    }
  };
  
  class CRequestEvent : public CEvent
  {
    public:
    core::CEventType& getType() const
    {
      return event::REQUEST;
    }
  };

  class COnErrorListener : public core::COnErrorListener
  {
    
  };
}

#endif // __net_cevent_hpp__
