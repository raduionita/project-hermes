#include <core\CListener.hpp>

namespace core
{
  CListener::CListener()
  {
    CEventManager::getInstance()->addListener(this);
  }
}
