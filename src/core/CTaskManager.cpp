#include <core/CTaskManager.hpp>

namespace core
{
  size_t CTaskManager::sTasks = 0;

  CTask::CTask(std::function<void(void)>&& callback)
  : mCallback(nullptr), mThread(nullptr), mDone(false)
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
  
  CTaskManager::CTaskManager()
  {
    log::info << "core::CTaskManager::CTaskManager()" << log::endl;
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
  
  const CTask& CTaskManager::run(std::function<void(void)>&& callback)
  {
    log::info << "core::CTaskManager::run(callback)" << log::endl;
    
    auto& sInstance = CTaskManager::getInstance();
    
    auto res = sInstance.mTasks.insert(std::make_pair(std::move(new CTask(std::move(callback))), false));
    
    return *(res.first->first);
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
}
