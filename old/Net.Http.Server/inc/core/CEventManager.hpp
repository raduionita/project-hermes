#ifndef __core_ceventmanager_hpp__
#define __core_ceventmanager_hpp__

#include <algorithm>
#include <vector>
#include <functional>

#include <core/CEvent.hpp>
#include <core/CListener.hpp>
#include <core/CSingleton.hpp>

namespace core
{
  class CListener;

  class CEventManager : public CSingleton<CEventManager>
  {
    friend class CSingleton<CEventManager>;
  
    protected:
    std::vector<CListener*> mListeners;
  
    public:
    void addListener(CListener* pListener);
    
    void trigger(CEvent&& event);
  };
}

#endif // __core_ceventmanager_hpp__
