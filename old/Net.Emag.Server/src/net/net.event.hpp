#ifndef __net_event_hpp__
#define __net_event_hpp__

#include <vector>

#include "net.hpp"

namespace net
{
  class COnRecvEventListener;
  class COnSendEventListener;

  enum class EEventType : unsigned int
  {
    INIT  = 0x0001,
    RUN   = 0x0002,
    SEND  = 0x0004,
    RECV  = 0x0008,
    CLOSE = 0x0010
  };

  class CEvent
  {
    public:
    CEvent()
    {
      
    }
    
    virtual ~CEvent()
    {
      
    }
    
    public:
    virtual net::EEventType getType() const = 0;
  };
  
  class CInitEvent : public CEvent
  {
    public:
    net::EEventType getType() const
    {
      return net::EEventType::INIT;
    }
  };
  
  class CSendEvent : public CEvent
  {
    friend class COnSendEventListener;
  
    public:
    net::CMessage* mMessage;
  
    public:
    CSendEvent() : mMessage(nullptr)
    {
    
    }
    
    ~CSendEvent()
    {
      delete mMessage;
    }
    
    public:
    net::EEventType getType() const
    {
      return net::EEventType::SEND;
    }
  };
  
  class CRecvEvent : public CEvent
  {
    friend class COnRecvEventListener;
  
    public:
    net::CMessage* mMessage;
  
    public:
    CRecvEvent() : mMessage(nullptr)
    {
    
    }
    
    ~CRecvEvent()
    {
      delete mMessage;
    }
    
    public:
    net::EEventType getType() const
    {
      return net::EEventType::RECV;
    }
  
    net::CMessage* getMessage() const
    {
      return mMessage;
    }
    
    void setMessage(CMessage* pMessage)
    {
      mMessage = pMessage;
    }
  };

  class CEventListener
  {
    public:
    virtual void trigger(CEvent* pEvent) = 0;
    
    virtual net::EEventType getType() const = 0;
  };
  
  class COnSendEventListener : public CEventListener
  {
    public:
    void trigger(CEvent* pEvent)
    {
      net::CSendEvent* pSendEvent = dynamic_cast<net::CSendEvent*>(pEvent);
      onSend(pSendEvent);
    }
    
    net::EEventType getType() const
    {
      return net::EEventType::RECV;
    }
  
    virtual void onSend(net::CSendEvent* pEvent) = 0;
  };
  
  class COnRecvEventListener : public CEventListener
  {
    public:
    void trigger(CEvent* pEvent)
    {
      net::CRecvEvent* pRecvEvent = dynamic_cast<net::CRecvEvent*>(pEvent);
      onRecv(pRecvEvent);
    }
    
    net::EEventType getType() const
    {
      return net::EEventType::RECV;
    }
  
    virtual void onRecv(net::CRecvEvent* pEvent) = 0;
  };

  class CEventManager
  {
    protected:
    std::vector<net::CEventListener*> mEventListeners;
    
    public:
    CEventManager()
    {
      
    }
    
    virtual ~CEventManager()
    {
      
    }
  
    void addEventListener(net::CEventListener* pEventListener)
    {
      mEventListeners.push_back(pEventListener);
    }
    
    void trigger(net::CEvent* pEvent)
    {
      for(net::CEventListener* pEventListener : mEventListeners)
        if(pEventListener->getType() == pEvent->getType())
          pEventListener->trigger(pEvent);
    }
  };
}

#endif // __net_event_hpp__
