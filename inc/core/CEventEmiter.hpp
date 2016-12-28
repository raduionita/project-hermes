#ifndef __core_ceventemiter_hpp__
#define __core_ceventemiter_hpp__

#include <core/CEvent.hpp>
#include <core/CTaskManager.hpp>

#include <log/CLogger.hpp>

#include <utility>
#include <functional>
#include <vector>
#include <iostream>

namespace core
{
  class CEventEmiter
  {
    typedef std::function<void(const CEvent*)> callback_t;

    protected:
    std::vector<std::pair<label_t, callback_t>> mActions;

    public:
    CEventEmiter()
    {

    }

    CEventEmiter(const CEventEmiter& that)
    {
      mActions = that.mActions;
    }

    CEventEmiter(CEventEmiter&& that)
    {
      mActions = std::move(that.mActions);
    }

    virtual ~CEventEmiter()
    {

    }

    protected:
    virtual const CTask& emit(const CEvent* e)
    {
      log::info << "> Emitting [" << e->mLabel << "] event..." << log::endl;
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


#endif // __core_ceventemiter_hpp__
