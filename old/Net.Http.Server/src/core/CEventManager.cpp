#include <core/CEventManager.hpp>

namespace core
{
  void CEventManager::addListener(CListener* pListener)
  {
    mListeners.push_back(pListener);
  }
    
  void CEventManager::trigger(CEvent&& event)
  {
    for(auto it = mListeners.begin(); it != mListeners.end(); ++it)
      if((*it)->getType() == event.getType())
        (*it)->trigger(event);
  }
}
