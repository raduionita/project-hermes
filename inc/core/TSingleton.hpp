#ifndef __core_tsingleton_hpp__
#define __core_tsingleton_hpp__

#include <core.hpp>

namespace core
{
  template<class T>
  class TSingleton
  {
    protected:
    static T sInstance;
    
    public:
    static T& getInstance() //@TODO: add to a monitoring CNgin class that will delete them all at the end...
    {
      return sInstance;
    }
    
    protected:
    TSingleton()
    {
      // sys::info << "sys::TSingleton::TSingleton()" << sys::endl;
    }
    
    virtual ~TSingleton()
    {
      //sys::info << "sys::TSingleton::TSingleton()" << sys::endl;
    }
  };
  template<class T>
  T TSingleton<T>::sInstance;
}

#endif // __core_singleton_hpp__
