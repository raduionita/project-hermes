#ifndef __core_ceventlistener_hpp__
#define __core_ceventlistener_hpp__

#include <core/CEvent.hpp>

#include <map>
#include <utility>
#include <future>
#include <functional>
#include <iostream>

namespace core
{
  class CEventListener
  {
    protected:
    std::map<label_t, std::function<void(CEvent&)>> mActions;
    
    public:
    CEventListener() 
    {
    
    }
    
    CEventListener(const CEventListener& that)
    {
      mActions = that.mActions;
    }
    
    virtual ~CEventListener()
    {
      
    }
     
    protected:
    virtual void trigger(CEvent&& e)
    {
      std::async(std::launch::async, [this, &e](void) {
        auto it = mActions.find(e.mLabel);
        if(it == mActions.end())
          return;
        it->second(e);
      });
    }
  };
}


#endif // __core_ceventlistener_hpp__
