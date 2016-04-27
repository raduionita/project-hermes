#ifndef __net_cserver_hpp__
#define __net_cserver_hpp__

#include <functional>

#include <net.hpp>

namespace net
{
  class CServer
  {
    protected:
    
    
    
    public:
    CServer(net::EProtocol);
    
    virtual ~CServer();
    
    CServer(const CServer&);
    
    CServer& operator = (const CServer&);
    
    public:
    CServer& listen(char const* host, ushort port, std::function<void(void)> callback); // host + port + callback
    CServer& listen(char const* host);                                                  // host + rand port + no callback
    CServer& listen(ushort port, std::function<void(void)> callback);                   // localhost + port + callback
    CServer& listen(ushort port);                                                       // localhost + port + no callback
    CServer& listen(std::function<void(void)> callback);                                // localhost + rand port + callback
    CServer& listen();                                                                  // localhost + rand port + no callback
  };
}

#endif // __net_cserver_hpp__
