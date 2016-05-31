#ifndef __http_amiddleware_hpp__
#define __http_amiddleware_hpp__

#include <http/CServer.hpp>

namespace http
{
  class AMiddleware
  {
    public:
    virtual void use(CServer& server) = 0;
  };
}

#endif // __http_amiddleware_hpp__
