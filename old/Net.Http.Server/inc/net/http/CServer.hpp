#ifndef __net_http_cserver_hpp__
#define __net_http_cserver_hpp__

#include <net/CServer.hpp>
#include <net/http/CRequest.hpp>
#include <net/http/CResponse.hpp>
#include <net/http.hpp>

namespace net { namespace http 
{
  class CApp;

  class CServer : public net::CServer
  {
    friend class CApp;
  
    protected:
    CRequest  mRequest;
    CResponse mResponse;
  
    public:
    CServer();
  };
}}

#endif // __net_http_cserver_hpp__
