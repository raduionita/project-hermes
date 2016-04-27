#ifndef __net_cmessageexception_hpp__
#define __net_cmessageexception_hpp__

#include <net.hpp>
#include <core/CException.hpp>
#include <net/CError.hpp>

#include <sstream>

namespace net
{
  class CMessageException : public core::CException
  {
    using core::CException::CException;
    
    public:
    CMessageException(const CError& err, const char* file, int line) : core::CException("", file, line)
    {
      std::stringstream ss;
      ss << err;
      mMessage = ss.str();
    }
  };
}

#endif // __net_cmessageexception_hpp__
