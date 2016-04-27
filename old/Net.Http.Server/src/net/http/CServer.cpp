#include <net/http/CServer.hpp>

namespace net { namespace http 
{
  CServer::CServer(void) : net::CServer(net::EProtocol::TCP)
  {
    
  }
}}
