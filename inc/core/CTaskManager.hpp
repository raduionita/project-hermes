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
    void close(CTask* task);
    
    public:
    static const CTask& run(std::function<void(void)>&& callback);
  
    static void wait(std::function<bool(void)>&& condition);            // if condition is true
    static void wait(size_t sec, std::function<bool(void)>&& callback); // for every sec run callback
  };
  size_t CTaskManager::sTasks = 0;
  
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
  
  CTaskManager::CTaskManager()
  {
    log::info << "core::CTaskManager::CTaskManager()" << log::endl;
    log::info << "> Thread count: " << std::thread::hardware_concurrency() << log::endl;
  }
  
  CTaskManager::~CTaskManager()
  {
    log::info << "core::CTaskManager::~CTaskManager()" << log::endl;
    
    for(auto it = mTasks.begin(); it != mTasks.end(); ++it)
    {
      it->second = true;
      it->first->join();
      delete it->first;
    }
  }
  
  void CTaskManager::close(CTask* task)
  {
    // @todo Need better closed task management
    log::info << "core::CTaskManager::close(task)" << log::endl;
    //mTasks[task] = false;
    task->close();
    if(mTasks[task] == false)
    {
      delete task;
      mTasks.erase(task);
      task = nullptr;
    }
  }

  const CTask& CTaskManager::run(std::function<void(void)>&& callback)
  {
    log::info << "core::CTaskManager::run(callback)" << log::endl;
    
    auto& sInstance = CTaskManager::getInstance();
    
    auto res = sInstance.mTasks.insert(std::make_pair(std::move(new CTask(std::move(callback))), false));
    
    return *(res.first->first);
  }

  CTask::CTask(std::function<void(void)>&& callback) : mCallback(nullptr), mThread(nullptr), mDone(false)
  {
    log::info << "core::CTask::CTask(callback)" << log::endl;
    
    mCallback = new std::function<void(void)>(std::move(callback));
    
    mId     = ++CTaskManager::sTasks;
    mThread = new std::thread([this](void) {
      (*mCallback)();
      log::info << "> Done with thread " << std::this_thread::get_id() << log::endl;
      CTaskManager::getInstance().close(this);
    });
    mThread->detach();
  }
  
  CTask::~CTask()
  {
    log::info << "core::CTask::~CTask() ["<< mId << "]" << log::endl;

    delete mCallback;
  }
  
  void CTask::join()
  {
    log::info << "core::CTask::join() ["<< mId << "]";
    int i = 0;
    while(mDone == false)
      ++i; // wait unti done is triggered
    log::info << " waited " << i << " loops" << log::endl;
  }

  void CTask::close()
  {
    mDone = true;
  }
} 

#endif // __core_ctaskmanager_hpp__
