#ifndef __core_ceventemmiter_hpp__
#define __core_ceventemmiter_hpp__

#include <core/CEvent.hpp>
#include <core/CTaskManager.hpp>

#include <log/CLogger.hpp>

#include <utility>
#include <functional>
#include <vector>
#include <iostream>

namespace core
{
  class CEventEmmiter
  {
    typedef std::function<void(const CEvent*)> callback_t;
  
    protected:
    std::vector<std::pair<label_t, callback_t>> mActions;
    
    public:
    CEventEmmiter() 
    {
    
    }
    
    CEventEmmiter(const CEventEmmiter& that)
    {
      mActions = that.mActions;
    }
    
    CEventEmmiter(CEventEmmiter&& that)
    {
      mActions = std::move(that.mActions);
    }
    
    virtual ~CEventEmmiter()
    {
      
    }

    protected:
    virtual const CTask& emit(const CEvent* e)
    {
      return CTaskManager::run([this, e] (void) {
        // one action after another for this event...to ensure 
        log::info << "> Thread " << std::this_thread::get_id() << log::endl;
        for(auto action : mActions)
          if(action.first == e->mLabel)
            action.second(e);
        delete e;
      });
    }
    
    virtual void add(const label_t& label, std::function<void(const CEvent*)>&& callback)
    {
      mActions.push_back(std::make_pair(label, std::move(callback)));
    }
  };
}


#endif // __core_ceventemmiter_hpp__
