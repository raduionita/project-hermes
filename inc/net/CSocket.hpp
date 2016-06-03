#ifndef __net_csocket_hpp__
#define __net_csocket_hpp__

#include <core/CException.hpp>
#include <net.hpp>
#include <net/CError.hpp>

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
    
    /**
     * Socket constructor factory from different parameters
     * @param EType          type        ESocket::CLIENT or ESocket::SERVER build strategy
     * @param unsigned short port        1337, 80, 8080
     * @param const char*    host        "localhost", "127.0.0.1" or NULL(binds to every ip)
     * @param EProtocol      protocol    EProtocol::TCP or EProtocol::UDP
     * @param EAddressType   addresstype EAddressType::IPV4 or EAddressType::IPV6
     * @param int            limit       =2147483647 | max allowed connections
     */
    CSocket(EType type, port_t port, const host_t& host, EProtocol protocol = EProtocol::TCP, EAddressType addresstype = EAddressType::UNSPEC, int limit = SOMAXCONN)
    : mSocket(INVALID_SOCKET)
    {
      log::info << "net::CSocket::CSocket(type, port, host, protocol, addresstype, limit)" << log::endl;
      mSocket = build(type, port, host, protocol, addresstype, limit);
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
    
    static socket_t build(EType type, port_t port, const host_t& host, EProtocol protocol = EProtocol::TCP, EAddressType addresstype = EAddressType::UNSPEC, int limit = SOMAXCONN)
    {
      log::info << "net::CSocket::build(type, port, host, protocol, addresstype, limit)" << log::endl;
      
      socket_t   sock = INVALID_SOCKET;
      ulong      temp = 1;
      int        yes  = 1;
      int        no   = 0;
      status_t   status;
      addrinfo_t hints,
               * srvinfo = NULL,
               * ptr     = NULL;
      ::memset(&hints, 0, sizeof(hints));
      hints.ai_family   = static_cast<int>(addresstype);                          // UNSPEC
      hints.ai_socktype = static_cast<int>(protocol == EProtocol::TCP ? ESocketType::TCP : ESocketType::UDP);
      hints.ai_flags    = type == EType::SERVER ? AI_PASSIVE : 0;                 // server
      hints.ai_protocol = static_cast<int>(protocol); // type == EType::CLIENT ? static_cast<int>(protocol) : 0; // client
      
      // use hints to get an address
      // NULL => 0.0.0.0 => bind to every ip on this machine
      status = ::getaddrinfo(host.size() == 0 ? NULL : host.c_str(), std::to_string((uint)port).c_str(), &hints, &srvinfo);
      if(status != STATUS_OK)
      {
        throw CException(net::getError(), __FILE__, __LINE__);
      }

      // find and bind/connect to an address
      for(ptr = srvinfo; ptr != NULL; ptr = ptr->ai_next)
      {
        //          socket(EAddressType,   ESocketType,      EProtocol); 
        sock = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if(sock == INVALID_SOCKET)
        {
          log::error << "> Error socket " << sock << ":" << gai_strerror(WSAGetLastError()) << "." << log::endl;
          continue;
        }
        
        ::setsockopt(sock, SOL_SOCKET,   SO_REUSEADDR, (char*)(&yes), sizeof(int));  // prevent "address in use" error
        ::setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY,  (char*)(&no),  sizeof(int));  // disable ipv6 only
        
        // non-blocking
#ifdef _WIN32_WINNT
        temp = 1;
        ::ioctlsocket(sock, FIONBIO, &temp);
#else // *UNIX
        temp = ::fcntl(sock, F_GETFL, 0);
        ::fcntl(sock, F_SETFL, temp | O_NONBLOCK);
#endif // _WIN32_WINNT

        // bind/connect
        if(type == EType::CLIENT)
        {
          status = ::connect(sock, ptr->ai_addr, (int)(ptr->ai_addrlen));
          if(status == SOCKET_ERROR)
          {
            log::error << "> Error connect: " << gai_strerror(WSAGetLastError()) << "." << log::endl;
            ::closesocket(sock);
            sock = INVALID_SOCKET;
            continue;
          }
          log::info << "> Connection to socket " << sock << "." << log::endl;
        }
        else // EType::SERVER
        {
          status = ::bind(sock, ptr->ai_addr, ptr->ai_addrlen);
          if(status == STATUS_OK)
          {
            char      host[NI_MAXHOST];
            char      serv[NI_MAXSERV];
            socklen_t alen = ptr->ai_family == EAddressType::IPV4 ? sizeof(sockaddr_in_t) : sizeof(sockaddr_in6_t);
            status = ::getnameinfo(ptr->ai_addr, alen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICSERV | NI_NUMERICHOST);
            if (status == STATUS_OK)
            {
              host[NI_MAXHOST-1] = '\0';
              serv[NI_MAXSERV-1] = '\0';
              log::info << "> Binding to socket " << sock << " on " << host << ":" << serv << "." << log::endl;
            }
            else
            {
              log::error << "> Error getnameinfo. " << ::gai_strerror(::WSAGetLastError()) << log::endl;
            }
          }
          else // SOCKET_ERROR
          {
            log::error << "> Error bind: " << ::gai_strerror(::WSAGetLastError()) << "." << log::endl;
            ::closesocket(sock);
            sock = INVALID_SOCKET;
            continue;
          }
        }
        break;
      }

      // if no addrinfo OR no socket => error
      if(ptr == NULL || sock == INVALID_SOCKET)
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
          if(::listen(sock, limit) != SOCKET_OK)
          {
            throw CException(::gai_strerror(WSAGetLastError()), __FILE__, __LINE__);
          }
          if(ptr->ai_family == EAddressType::IPV4)
          {
            sockaddr_in_t sin;
            socklen_t len = sizeof(sin);
            ::getsockname(sock, (sockaddr_t*)(&sin), &len);
            log::info << "> Listening to socket " << sock << " on port " << ::ntohs(sin.sin_port) << "." << log::endl;
          }
          else 
          {
            sockaddr_in6_t sin6;
            socklen_t len = sizeof(sin6);
            ::getsockname(sock, (sockaddr_t*)(&sin6), &len);
            log::info << "> Listening to socket " << sock << " on port " << ::ntohs(sin6.sin6_port) << "." << log::endl;

          }
        }
      }
      
      return sock;
    }
    
    // getHost()
    // getPort()
    // getSockaddr()
  };
}

#endif // __net_csocket_hpp__
