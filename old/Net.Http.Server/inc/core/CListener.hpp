#ifndef __core_CEVENTLISTENER_HPP__
#define __core_CEVENTLISTENER_HPP__

#include <functional>

#include <core/CEvent.hpp>
#include <core/CEventManager.hpp>

namespace core
{
  class CListener
  {
    friend class CEventManager;
  
    public:
    CListener();
    
    protected:
    virtual CEventType getType() const = 0;
    
    virtual void trigger(CEvent& event) = 0;
  };
  
  class COnErrorListener : public CListener
  {
    friend class CEventManager;
  
    protected:
    CEventType getType() const
    {
      return event::ERROR;
    }
    
    void trigger(CEvent& event)
    {
      CErrorEvent& errorevent = dynamic_cast<CErrorEvent&>(event);
      onError(errorevent);
    }
    
    virtual void onError(CErrorEvent& event) = 0;
  };
  
  class COnTickListener : public CListener
  {
    friend class CEventManager;
  
    protected:
    CEventType getType() const
    {
      return event::TICK;
    }
    
    void trigger(CEvent& event)
    {
      CTickEvent& tickevent = dynamic_cast<CTickEvent&>(event);
      onTick(tickevent);
    }
    
    virtual void onTick(CTickEvent& event) = 0;
  };
}

#endif // __core_CEVENTLISTENER_HPP__
