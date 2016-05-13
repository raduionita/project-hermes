#ifndef __core_csynchronizable_hpp__
#define __core_csynchronizable_hpp__

#include <mutex>

#include <log.hpp>

#define synchronized(mtx) for(std::unique_lock<CSynchronizable> ulck(mtx); ulck; ulck.unlock())

namespace core
{
  class CSynchronizable
  {
    protected:
    std::mutex mMutex;
    
    CSynchronizable()
    {
      log::info << "core::CSynchronizable::CSynchronizable()" << log::endl;
    }
    
    virtual ~CSynchronizable()
    {
      log::info << "core::CSynchronizable::~CSynchronizable()" << log::endl;
    }
    
    public:
    bool lock()
    {
      mMutex.lock();
      log::info << "core::CSynchronizable::lock() [" << std::this_thread::get_id() << "]" << log::endl;
      return true;
    }
    
    bool unlock()
    {
      mMutex.unlock();
      log::info << "core::CSynchronizable::unlock() [" << std::this_thread::get_id() << "]" << log::endl;
      return true;
    }
  };
}

#endif // __core_csynchronizable_hpp__
