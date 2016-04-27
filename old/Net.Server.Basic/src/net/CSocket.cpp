#include <net/CSocket.hpp>

namespace net
{
  using namespace core;

  CSocket::CSocket(EType type, port_t port, const host_t& host, EProtocol protocol, EAddressType addresstype, int limit)
  : mSocket(INVALID_SOCKET)
  {
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
        std::cout << "> Error socket " << mSocket << ":" << gai_strerror(WSAGetLastError()) << "." << std::endl;
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
          std::cout << "> Error connect: " << gai_strerror(WSAGetLastError()) << "." << std::endl;
          ::closesocket(mSocket);
          mSocket = INVALID_SOCKET;
          continue;
        }
        std::cout << "> Connection to socket " << mSocket << "." << std::endl;
      }
      else // EType::CLIENT
      {
        status = ::bind(mSocket, ptr->ai_addr, ptr->ai_addrlen);
        if(status == SOCKET_ERROR)
        {
          std::cout << "> Error bind: " << gai_strerror(WSAGetLastError()) << "." << std::endl;
          ::closesocket(mSocket);
          mSocket = INVALID_SOCKET;
          continue;
        }
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        ::getnameinfo(ptr->ai_addr, sizeof(sockaddr_t), host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICSERV);
        host[NI_MAXHOST-1] = '\0';
        serv[NI_MAXSERV-1] = '\0';
        std::cout << "> Binding to socket " << mSocket << " on " << host << ":" << serv << "." << std::endl;
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
        std::cout << "> Listening to socket " << mSocket << " on port " << ::ntohs(sin.sin_port) << "." << std::endl;
      }
    }
  }

  CSocket::CSocket(socket_t socket) 
  : mSocket(socket)
  {
    sockaddr_in sin;
    socklen_t len = sizeof(sin);
    ::getsockname(mSocket, (sockaddr_t*)(&sin), &len);
    std::cout << "> Binding to socket " << mSocket << " on port " << ::ntohs(sin.sin_port) << "." << std::endl;
  }
  
  CSocket::CSocket(const CSocket& that)
  {
    mSocket = that.mSocket;
  }

  CSocket::~CSocket()
  {
    //close();
  }

  CSocket& CSocket::operator =(const CSocket& that)
  {
    if(this != &that)
    {
      mSocket = that.mSocket;
    }
    return *this;
  }
  
  CSocket::operator socket_t() const
  {
    return mSocket;
  }
  
  bool CSocket::operator ==(socket_t socket) const
  {
    return mSocket == socket;
  }
  
  void CSocket::close()
  {
    // OR ::close()
    ::closesocket(mSocket);
    std::cout << "> Closing socket " << mSocket << "." << std::endl;
  }

  bool CSocket::bound()
  {
    return mSocket != INVALID_SOCKET;
  }
}
