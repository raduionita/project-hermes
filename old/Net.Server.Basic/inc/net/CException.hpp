#ifndef __cexception_hpp__
#define __cexception_hpp__

#include <core.hpp>
#include <net.hpp>
#include <net/CError.hpp>

namespace net
{
  class CException : public core::CException
  {
    using core::CException::CException;
    
    public:
    CException(const char* file, int line) : core::CException("", file, line)
    {
      CError err;
      std::stringstream ss;
      ss << err;
      mMessage = ss.str();
    }
  };
}

#endif // __cexception_hpp__
