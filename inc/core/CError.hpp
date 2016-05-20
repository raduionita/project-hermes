#ifndef __core_cerror_hpp__
#define __core_cerror_hpp__

#include <core.hpp>

namespace core
{
  class CError
  {
    protected:
    int         mCode;
    std::string mMessage;
    
    public:
    CError() : mCode(0)
    {
      
    }
    
    CError(int code, const std::string& message) : mCode(code), mMessage(message)
    {
      
    }
    
    virtual ~CError()
    {
      
    }
  
    bool operator ==(int code) const
    {
      return mCode == code;
    }
    
    bool operator !=(int code) const
    {
      return mCode != code;
    }
    
    friend std::iostream& operator <<(std::iostream& out, const CError& error);
  };
  
  std::iostream& operator <<(std::iostream& out, const CError& error)
  {
    out << "Error [" << error.mCode << "] " << error.mMessage;
    return out;
  }
}

#endif // __core_cerror_hpp__
