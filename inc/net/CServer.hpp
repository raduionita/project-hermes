#ifndef __net_CSERVER_HPP__
#define __net_CSERVER_HPP__

#include <net.hpp>
#include <net/CSocket.hpp>
#include <net/CError.hpp>
#include <core/CException.hpp>

namespace net
{
  class AServer
  {
    public:
    AServer()
    {
      log::info << "net::AServer::AServer()" << log::endl;
#ifdef _WIN32_WINNT
      log::info << "> Init Windows Sockets." << log::endl;
      static WSADATA  wsadata;
      static status_t wsastatus = ::WSAStartup(MAKEWORD(2,2), &wsadata);
      if(wsastatus != STATUS_OK)
      {
        throw core::CException(::gai_strerror(::WSAGetLastError()), __FILE__, __LINE__);
      }
#else // LINUX
      // do nothig
#endif // _WIN32_WINNT
    }
    
    virtual ~AServer()
    {
      log::info << "net::AServer::~AServer()" << log::endl;
#ifdef _WIN32_WINNT
      WSACleanup();
      log::info << "> Clean Windows Sockets." << log::endl;
#else // LINUX
      // do nothing
#endif
    }
  };
  
  class CServer : public AServer // TCP & UDP
  {
    public:
    CServer(EProtocol protocol)
    {
      
    }
    
    public:
    //CSocket& bind()
    
    CServer& listen()
    {
      return *this;
    }
  };
  
  class Client
  {
  
  };
}

#endif // __net_CSERVER_HPP__
