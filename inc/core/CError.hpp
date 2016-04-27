#ifndef __core_cerror_hpp__
#define __core_cerror_hpp__

#include <core.hpp>

#include <iostream>
#include <string>

namespace core
{
  class CError
  {
    public:
    enum class ESeverity : ushort
    {
      WARNING = 0, // notice, depricated
    //ERROR   = 1,
      FATAL   = 2  //
    };

    public:
    ESeverity   mSeverity;
    std::string mMessage;
    
    public:
    CError() : mSeverity(ESeverity::FATAL)
    {
    
    }
    
    virtual ~CError()
    {
      
    }
    
    friend std::ostream& operator << (std::ostream&, const CError&);
  };
}

#endif //__core_cerror_hpp__
