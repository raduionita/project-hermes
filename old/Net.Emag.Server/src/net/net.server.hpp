#ifndef __net_server_hpp__
#define __net_server_hpp__

#include <cstring>
#include <string>
#include <ctime>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "net.hpp"
#include "net.event.hpp"

#define MAXBUFLEN 100

namespace net
{
  class CConnection;

  class CSocket
  {
    protected:
    SOCKET mSocket;
    
    public:
    CSocket()
    {
    
    }
    
    CSocket(SOCKET socket) 
    : mSocket(socket)
    {
    
    }
    
    virtual ~CSocket()
    {
      close();
    }
    
    CSocket& operator = (const CSocket& that)
    {
      if(this == &that)
        mSocket = that.mSocket;
      return *this;
    }
    
    CSocket& operator = (const SOCKET& socket)
    {
      mSocket = socket;
      return *this;
    }
    
    public:
    void close()
    {
      ::closesocket(mSocket);
    }
  };
  
  class CConnectionStrategy // driver
  {
    friend class CConnection;
  
    protected:
    net::CAddress mAddress;
    const char*   mPort;
    socket_t      mSocket;
    
    public:
    CConnectionStrategy(const CAddress& address, const char* port = NULL)
    : mAddress(address), mPort(port)
    {
    
    }
    
    public:
    virtual socket_t     connect() = 0;
    virtual net::CInput* recv() = 0;
    virtual void         send(net::COutput*) = 0;
    
    void close() 
    {
      ::closesocket(mSocket);
      ::WSACleanup();
    }
  };

  class CTcpConnectionStrategy : public CConnectionStrategy
  {
    friend class CConnection;
    
    public:
    CTcpConnectionStrategy(const CAddress& address, const char* port = NULL) : CConnectionStrategy(address, port)
    {
    
    }
  
    socket_t     connect() { return 1; }
    net::CInput* recv() { return nullptr; }
    void         send(net::COutput*) { }
  };
  
  class CUdpConnectionStrategy : public CConnectionStrategy
  {
    friend class CConnection;
  
    public:
    CUdpConnectionStrategy(const CAddress& address, const char* port = NULL) : CConnectionStrategy(address, port)
    {
    
    }
  
    socket_t connect()
    {
      std::cout << "CTcpConnectionStrategy::connect()" << std::endl;
    
      WSAData _wsadata;
      int     _what   = WSAStartup(MAKEWORD(2, 2), &_wsadata);
      
      if(_what != 0)
      {
        // std::cerr << "> Failed. Error: " << WSAGetLastError() << std::endl;
        throw net::CException(gai_strerror(WSAGetLastError()));
      }
    
      addrinfo_t  _hints;
      addrinfo_t* _srvinfo;
      addrinfo_t* _addr;    // _server_addr;
      const char  _yes = '1';
      int         _status;
      char        _ipstr[INET6_ADDRSTRLEN];
      
      memset(&_hints, 0, sizeof(_hints));
      _hints.ai_family   = AF_INET;     // IPV4, IPV6 or ALL
      _hints.ai_socktype = SOCK_DGRAM;  // TPC or UDP
      _hints.ai_flags    = AI_PASSIVE;  // passive = server
      
      if((_status = ::getaddrinfo(mAddress, mPort, &_hints, &_srvinfo)) != 0)
      {
        // TODO: replace with throw CException
        std::cerr << "> getaddrinfo" << std::endl;
        throw net::CException(::gai_strerror(_status));
      }
      
      for(_addr = _srvinfo; _addr != NULL; _addr = _addr->ai_next)
      {
        if((mSocket = ::socket(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol)) == SOCKET_ERROR)
        {
          // TODO: log
          // net::CLogger::DEBUG << "> socket no good" << std::endl;
          continue;
        }
        
        // for TCP & UDP...!?
        ::setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &_yes, sizeof(_yes));
        
        if(::bind(mSocket, _addr->ai_addr, _addr->ai_addrlen) == SOCKET_ERROR)
        {
          close();
          // TODO: log
          // net::CLogger::DEBUG << "> bind failed" << std::endl;
          continue;
        }
        
        break; // found socket & bound it...exit loop
        
        /* list host address
        void* _addr;
        const char* _ipver;
        
        if(_it->ai_family == EAddressType::IPV4)
        {
          SOCKADDR_IN* _ipv4 = (SOCKADDR_IN*) _it->ai_addr;
          _addr = &(_ipv4->sin_addr);
          _ipver = "IPv4";
        }
        else
        {
          SOCKADDR_IN6* _ipv6 = (SOCKADDR_IN6*) _it->ai_addr;
          _addr = &(_ipv6->sin6_addr);
          _ipver = "IPv6";
        }
        
        inet_ntop(_it->ai_family, _addr, _ipstr, sizeof(_ipstr));
        std::cout << "> " << _ipver << " " << _ipstr << std::endl;
        */
      }
      
      if(_addr == NULL) // no address found
      {
        throw net::CException("No connection address found!");
      }
    
      // trigger(onBind)
    
      ::inet_ntop(_addr->ai_family, _addr, _ipstr, sizeof(_ipstr));
      std::cout << "> [" << _ipstr << ":" << mPort << "] listening..."  << std::endl;
      
      ::freeaddrinfo(_srvinfo);
      
      //////////////////////////////////////////////////////////////////////
      
      ::fd_set _reads;
      //::fd_set _writes;
      
      timeval_t tv = { 1, 0 };
      
      while (true)
      {
        FD_ZERO(&_reads);
        //FD_ZERO(&_writes);
        FD_SET(mSocket, &_reads);
        
        _status = select(mSocket+1, &_reads, NULL, NULL, &tv);
        if(_status == -1)
        {
          // error
          std::cerr << "> error select: " << net::getErrorString(WSAGetLastError()) << std::endl;
          close();
          break;
        }
        else if(_status == 0) 
        {
          // timeout
        }
        else // _status == 1
        {
          // receives message
        }
        
        if(FD_ISSET(mSocket, &_reads))
        {
          sockaddr_storage_t _addr; // _client_addr;
          int  _addrlen = sizeof(_addr);
          char _input[MAXBUFLEN];
          int  _bytes = 0;
          char _ipstr[INET6_ADDRSTRLEN];
          if((_bytes = ::recvfrom(mSocket, _input, MAXBUFLEN - 1, 0, (sockaddr_t*)(&_addr), &_addrlen)) == SOCKET_ERROR)
          {
            std::cerr << "> error recvfrom" << std::endl;
            close();
            break;
          }
          
          _input[_bytes] = '\0';
          
          // trigger(onRecv) or trigger(onMessage) or trigger(onInput)
          
          inet_ntop(_addr.ss_family, ::get_in_addr((sockaddr_t*)(&_addr)), _ipstr, sizeof(_ipstr));
          
          std::cout << "> [" << _ipstr << "] " << _input << std::endl;
          
          if(::sendto(mSocket, _input, _bytes, 0, (sockaddr_t*)(&_addr), _addrlen) != _bytes)
          {
            std::cerr << "> error sendto: " << net::getErrorString(WSAGetLastError()) << std::endl;
            close();
            break;
          }
        }
      }
      
      
      
      
      
      
      // send
//      size_t      _bytes  = pOutput->mMessage.mMessage.size();
//      const char* _output = pOutput->mMessage.mMessage.c_str();
//      if(::sendto(mSocket, _output, _bytes, 0, (sockaddr_t*)(&_addr), _addrlen) != _bytes)
//      {
//        std::cerr << "> error sendto" << std::endl;
//        close();
//        return;
//      }
      
      
      
      
      
      
      
      
      return mSocket;
    }
    
    net::CInput* recv()              { return nullptr; }
    void         send(net::COutput*) { }
  };

  class CConnection
  {
    public:
    net::EProtocol       mProtocol;
    CConnectionStrategy* mConnectionStrategy;
    
    // TODO: replace with net::CSocket*
    socket_t             mSocket;
    
    public:
    CConnection(const CAddress& address, const char* port = NULL, const net::EProtocol protocol = net::EProtocol::TCP)
    : mProtocol(protocol)
    {
      switch(protocol)
      {
        case net::EProtocol::TCP:
          mConnectionStrategy = new CTcpConnectionStrategy(address, port);
        break;
        case net::EProtocol::UDP:
          mConnectionStrategy = new CUdpConnectionStrategy(address, port);
        break;
        default:
          throw net::CException("Not implemented!"); 
        break;
      }
    }
    
    public:
    void connect()
    {
      mSocket = mConnectionStrategy->connect();
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
    }
    
    void close()
    {
      mConnectionStrategy->close();
    }
  
    net::CInput* recv()
    {
      // receive

      
      return nullptr;
    }
    
    void send(net::COutput* pOutput)
    {

    }
    
    net::CAddress getAddress() const
    {
      return mConnectionStrategy->mAddress;
    }
    
    const char* getPort() const
    {
      return mConnectionStrategy->mPort;
    }
    
    net::EProtocol getProtocol() const
    {
      return mProtocol;
    }
  };

  class CServer : public CEventManager
  {
    protected:
    net::CAddress  mAddress;
    const char*    mPort;
    net::EProtocol mProtocol;
    
    net::EStatus      mStatus;
    net::CConnection* mConnection;
  
    public:
    CServer()
    : CEventManager(), 
      mAddress(NULL), mPort(NULL), mProtocol(net::EProtocol::TCP), mStatus(net::EStatus::BAD), mConnection(nullptr)
    {
      trigger(new net::CInitEvent);
    }
    
    CServer(const char* address, const char* port = NULL, net::EProtocol protocol = net::EProtocol::TCP)
    : CEventManager(), 
      mAddress(address), mPort(port), mProtocol(protocol), mStatus(net::EStatus::BAD), mConnection(nullptr)
    {
      trigger(new net::CInitEvent);
      
      // SOCKET _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    
    virtual ~CServer()
    {
      close();
      delete mConnection;
    }
    
    protected:
    void connect()
    {
      if(mConnection == nullptr) 
      {
        mConnection = new net::CConnection(mAddress, mPort, mProtocol);
        mConnection->connect();
      }
      else
      {
        throw net::CException("Already connected!");
      }
    }
    
    public:
    void run()
    {
      // trigger: on run
      // trigger(new net::CRunEvent);
      
      connect();
      
      while(true)
      {
        recv();
        
        send();
        
        break;
      }
      
      close();
      
      return;
    }
    
    void recv()
    {
      mConnection->recv();
    
//      net::CMessage* pMessage = mConnection->recv();
//    
//      // trigger: on recv
//      net::CRecvEvent* pEvent = new net::CRecvEvent;
//      pEvent->mMessage = pMessage;
//      trigger(pEvent);
//      delete pEvent;
    }
    
    void send()
    {
      mConnection->send(nullptr);
    }
  
    void close()
    {
      // trigger: on close
      // trigger(new net::CCloseEvent);
      mConnection->close();
    }
  };
}

#endif // __net_server_hpp__
