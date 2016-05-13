#ifndef __core_ctaskmanager_hpp__
#define __core_ctaskmanager_hpp__

#include <core.hpp>
#include <core/TSingleton.hpp>

#include <log/CLogger.hpp>

#include <future>
#include <thread>
#include <functional>
#include <map>
#include <utility>
#include <iostream>

namespace core
{
  class CCallback : public std::function<void(void)>
  {
  
  };
  
  class CTask;
  
  class CTaskManager : public TSingleton<CTaskManager>
  {
    friend class CTask;
    friend class TSingleton<CTaskManager>;
  
    protected:
    static CTaskManager    sInstance;
    std::map<CTask*, bool> mTasks; // CTask, bIsInJoiningMode
    static size_t          sTasks;
    
    protected:
    CTaskManager();
    virtual ~CTaskManager();
    
    public:
    static const CTask& run(std::function<void(void)>&& callback);
    static void wait(std::function<bool(void)>&& condition);            // if condition is true
    static void wait(size_t sec, std::function<bool(void)>&& callback); // for every sec run callback
    void close(CTask* task);
  };
  
  class CTask
  {
    friend class CTaskManager;
  
    protected:
    std::function<void(void)>* mCallback;
    std::thread*               mThread;
    uint                       mId;
    bool                       mDone;
    
    // @todo Implement thread pool...save the callback, don't start the thread until one releases...
    
    public:
    CTask(std::function<void(void)>&& callback);
    virtual ~CTask();
    
    public:
    void close();
    void join();
  };
}

#endif // __core_ctaskmanager_hpp__
