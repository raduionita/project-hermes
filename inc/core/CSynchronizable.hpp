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
      log::debug << "core::CSynchronizable::CSynchronizable()" << log::endl;
    }

    virtual ~CSynchronizable()
    {
      log::debug << "core::CSynchronizable::~CSynchronizable()" << log::endl;
    }

    public:
    bool lock()
    {
      mMutex.lock();
      log::info << "> Locking [" << std::this_thread::get_id() << "]" << log::endl;
      return true;
    }

    bool unlock()
    {
      mMutex.unlock();
      log::info << "> Unlocking [" << std::this_thread::get_id() << "]" << log::endl;
      return true;
    }
  };
}

#endif // __core_csynchronizable_hpp__
