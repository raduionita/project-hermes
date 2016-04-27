#ifndef __net_connectionexception_hpp__
#define __net_connectionexception_hpp__

#include <net.hpp>
#include <net/CException.hpp>
#include <net/CError.hpp>

#include <sstream>

namespace net
{
  class CConnectionException : public net::CException
  {
    using net::CException::CException;
  };
}

#endif // __net_connectionexception_hpp__
