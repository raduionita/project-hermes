#ifndef __net__csocket_hpp__
#define __net__csocket_hpp__

#include <net.hpp>
#include <core/CException.hpp>

#include <iostream>

namespace net
{
  using namespace core;

  class CSocket
  {
    public:
    enum EType : ushort
    {
      SERVER = 0,
      CLIENT = 1
    };

    protected:
    socket_t mSocket;

    public:
    CSocket(EType type, port_t port, const host_t& host, EProtocol protocol, EAddressType addresstype, int limit = SOMAXCONN);
    CSocket(socket_t socket);
    CSocket(const CSocket&);
    virtual ~CSocket();
    
    CSocket& operator = (const CSocket&);
    operator socket_t () const;
    bool operator == (socket_t) const;
    
    public:
    void close();
    bool bound();
    
    // getHost()
    // getPort()
    // getSockaddr()
  };
}

#endif //__net__csocket_hpp__
