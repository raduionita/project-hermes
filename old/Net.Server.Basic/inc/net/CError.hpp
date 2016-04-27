#ifndef __net_cerror_hpp__
#define __net_cerror_hpp__

#include <net.hpp>
#include <core/CError.hpp>

namespace net
{
  class CError : public core::CError
  {
    public:
    //  mServerity;
    //  mMessage;
    int mCode;
  
    public:
    CError() : core::CError(), mCode(::WSAGetLastError())
    {
      mSeverity = ESeverity::FATAL;
      mMessage = std::string(::gai_strerror(mCode));
    }
    
    CError(int code) : core::CError(), mCode(code)
    {
      mSeverity = ESeverity::FATAL;
      mMessage = std::string(::gai_strerror(mCode));
    }
    
    CError(const std::string& msg) : mCode(-1)
    {
      mSeverity = ESeverity::WARNING;
      mMessage  = msg;
    }
    
    friend std::ostream& operator << (std::ostream&, const CError&);
  };
}

#endif // __net_cerror_hpp__
