#ifndef __core_singleton_hpp__
#define __core_singleton_hpp__

namespace core
{
  template<class T>
  class CSingleton
  {
    protected:
    static T* sInstance;
    
    public:
    static T*& getInstance() //@TODO: add to a monitoring CNgin class that will delete them all at the end...
    {
      if(sInstance == nullptr)
        sInstance = new T;
      return sInstance;
    }
    
    static void freeInstance()
    {
      if(sInstance != nullptr)
      {
        delete sInstance;
        sInstance = nullptr;
      }
    }
    
    protected:
    CSingleton()
    {
      // sys::info << "sys::CSingleton::CSingleton()" << sys::endl;
    }
    
    virtual ~CSingleton()
    {
      //sys::info << "sys::CSingleton::CSingleton()" << sys::endl;
    }
  };
  template<class T>
  T* CSingleton<T>::sInstance = nullptr;
}

#endif // __core_singleton_hpp__
