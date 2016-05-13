#ifndef __net_csocket_hpp__
#define __net_csocket_hpp__

#include <net.hpp>
#include <core/CException.hpp>

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
    
    enum ESocket
    {
      INVALID = INVALID_SOCKET,
      GOOD    = SOCKET_OK,
    };

    protected:
    socket_t mSocket;

    public:
    CSocket()
    : mSocket(INVALID_SOCKET)
    {
      log::info << "net::CSocket::CSocket()" << log::endl;
    }
    
    CSocket(socket_t socket)
    : mSocket(socket)
    {
      log::info << "net::CSocket::CSocket(socket)" << log::endl;
      
      if(socket != INVALID_SOCKET) 
      {
        sockaddr_in sin;
        socklen_t len = sizeof(sin);
        ::getsockname(mSocket, (sockaddr_t*)(&sin), &len);
        log::info << "> Binding to socket " << mSocket << " on port " << ::ntohs(sin.sin_port) << "." << log::endl;
      }
    }
    
    CSocket(EType type, port_t port, const host_t& host, EProtocol protocol, EAddressType addresstype, int limit = SOMAXCONN)
    : mSocket(INVALID_SOCKET)
    {
      log::info << "net::CSocket::CSocket(type, port, host, protocol, addresstype, limit)" << log::endl;
      
      status_t status;
      addrinfo_t hints,
               * srvinfo = NULL,
               * ptr     = NULL;
      ::memset(&hints, 0, sizeof(hints));
      hints.ai_family   = static_cast<int>(addresstype);                          // UNSPEC
      hints.ai_socktype = static_cast<int>(protocol == EProtocol::TCP ? ESocketType::TCP : ESocketType::UDP);
      hints.ai_flags    = type == EType::SERVER ? AI_PASSIVE : 0;                 // server
      hints.ai_protocol = type == EType::CLIENT ? static_cast<int>(protocol) : 0; // client

      // use hints to get an address
      // NULL => 0.0.0.0 => bind to every ip on this machine
      status = ::getaddrinfo(NULL, std::to_string((uint)port).c_str(), &hints, &srvinfo);
      if(status != STATUS_OK)
      {
        throw CException(::gai_strerror(WSAGetLastError()), __FILE__, __LINE__);
      }

      // find and bind/connect to an address
      for(ptr = srvinfo; ptr != NULL; ptr = ptr->ai_next)
      {
        //          socket(EAddressType,   ESocketType,      EProtocol);
        mSocket = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if(mSocket == INVALID_SOCKET)
        {
          log::error << "> Error socket " << mSocket << ":" << gai_strerror(WSAGetLastError()) << "." << log::endl;
          continue;
        }

        // prevent "address in use" error
        ::setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, "1", sizeof(int));

        // bind/connect
        if(type == EType::CLIENT)
        {
          status = ::connect(mSocket, ptr->ai_addr, (int)(ptr->ai_addrlen));
          if(status == SOCKET_ERROR)
          {
            log::error << "> Error connect: " << gai_strerror(WSAGetLastError()) << "." << log::endl;
            ::closesocket(mSocket);
            mSocket = INVALID_SOCKET;
            continue;
          }
          log::info << "> Connection to socket " << mSocket << "." << log::endl;
        }
        else // EType::CLIENT
        {
          status = ::bind(mSocket, ptr->ai_addr, ptr->ai_addrlen);
          if(status == SOCKET_ERROR)
          {
            log::error << "> Error bind: " << gai_strerror(WSAGetLastError()) << "." << log::endl;
            ::closesocket(mSocket);
            mSocket = INVALID_SOCKET;
            continue;
          }
          char host[NI_MAXHOST];
          char serv[NI_MAXSERV];
          ::getnameinfo(ptr->ai_addr, sizeof(sockaddr_t), host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICSERV);
          host[NI_MAXHOST-1] = '\0';
          serv[NI_MAXSERV-1] = '\0';
          log::info << "> Binding to socket " << mSocket << " on " << host << ":" << serv << "." << log::endl;
        }
        break;
      }

      // if no addrinfo OR no socket => error
      if(ptr == NULL || mSocket == INVALID_SOCKET)
      {
        throw CException(::gai_strerror(WSAGetLastError()), __FILE__, __LINE__);
      }

      // free srvinfo from memory
      ::freeaddrinfo(srvinfo);

      if(type == EType::CLIENT)
      {
        // do nothing...
      }
      else // EType::SERVER
      {
        if(protocol == EProtocol::TCP)
        {
          // server: put socket in listening mode
          if(::listen(mSocket, limit) != SOCKET_OK)
          {
            throw CException(::gai_strerror(WSAGetLastError()), __FILE__, __LINE__);
          }
          sockaddr_in sin;
          socklen_t len = sizeof(sin);
          ::getsockname(mSocket, (sockaddr_t*)(&sin), &len);
          log::info << "> Listening to socket " << mSocket << " on port " << ::ntohs(sin.sin_port) << "." << log::endl;
        }
      }
    }
    
    CSocket(const CSocket& that)
    {
      log::info << "net::CSocket::CSocket(that&)" << log::endl;
      
      mSocket = that.mSocket;
    }
    
    CSocket(CSocket&& that)
    {
      log::info << "net::CSocket::CSocket(that&&)" << log::endl;
      
      mSocket = std::move(that.mSocket);
    }
    
    virtual ~CSocket()
    {
      log::info << "net::CSocket::~CSocket()" << log::endl;
      //close();
      
    }
    
    CSocket& operator =(const CSocket& that)
    {
      log::info << "net::CSocket::operator=(that&)" << log::endl;
      
      if(this != &that)
      {
        mSocket = that.mSocket;
      }
      return *this;
    }
    
    CSocket& operator =(CSocket&& that)
    {
      log::info << "net::CSocket::operator=(that&&)" << log::endl;
      
      if(this != &that)
      {
        mSocket = std::move(that.mSocket);
      }
      return *this;
    }
    
    operator socket_t() const
    {
      return mSocket;
    }
    
    bool operator ==(socket_t socket) const
    {
      return mSocket == socket;
    }
    
    public:
    void close()
    {
      log::info << "net::CSocket::close()" << log::endl;
      // OR ::close()
      ::closesocket(mSocket);
      if(mSocket == CSocket::INVALID)
        log::info << "> Closing socket " << "INVALID" << "." << log::endl;
      else
        log::info << "> Closing socket " << mSocket << "." << log::endl;
    }

    bool bound()
    {
      return mSocket != INVALID_SOCKET;
    }
    
    // getHost()
    // getPort()
    // getSockaddr()
  };
}

#endif // __net_csocket_hpp__
