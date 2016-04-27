#include <net\CServer.hpp>

namespace net 
{
  CServer::CServer(net::EProtocol protocol)
  {
    
  }

  CServer::~CServer(void)
  {
    //dtor
  }

  CServer::CServer(const CServer& that)
  {
    //copy ctor
  }

  CServer& CServer::operator = (const CServer& that)
  {
    if(this == &that) return *this;
    
    
    
    return *this;
  }

  CServer& CServer::listen(ushort port, std::function<void(void)> callback)
  {
    // connect
    // setup event listeners: onTick onRecv onSend
  
    // connect to socket server
    // for(ever)
    //   check for incomming data(connections or messages)
    //   
    
    return *this;
  }
}
