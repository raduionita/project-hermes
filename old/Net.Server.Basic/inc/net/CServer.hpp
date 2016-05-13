#ifndef __net_cserver_hpp__
#define __net_cserver_hpp__

#include <core/CException.hpp>
#include <core/CEventListener.hpp>
#include <net.hpp>
#include <net/CSocket.hpp>
#include <net/CMessage.hpp>
#include <net/CMessageEvent.hpp>
#include <net/CError.hpp>
#include <net/CErrorEvent.hpp>
#include <net/CMessageException.hpp>
#include <net/CConnection.hpp>
#include <net/CConnectionEvent.hpp>
#include <net/CConnectionException.hpp>
#include <net/CCloseEvent.hpp>
#include <net/CRequest.hpp>
#include <net/CRequestEvent.hpp>
#include <net/CFlushEvent.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <map>
#include <utility>
#include <list>

namespace net
{
  using namespace core;
  
  class CServer : public CEventListener
  {
    protected:
    float     mTimeout;
    bool      mRunning;
    port_t    mPort;
    host_t    mHost;
    EProtocol mProtocol;
    CSocket*  mSocket;
    
    public:
    CServer(void)
    : CEventListener(),
      mTimeout(0.0f), mRunning(false), mPort(1337), mHost("localhost"), mProtocol(EProtocol::NONE), mSocket(nullptr)
    {
      // do nothing
    }

    CServer(port_t port, const host_t& host, EProtocol protocol)
    : CEventListener(),
      mTimeout(0.0f), mRunning(false), mPort(port), mHost(host), mProtocol(protocol), mSocket(nullptr)
    {
      // do nothing
    }

    virtual ~CServer(void)
    {
      free();
      std::cout << "> Closing. Error " << errno << ". " << ::gai_strerror(::WSAGetLastError()) << std::endl;
    }

    protected:
    void bind(void) throw(CException)
    {
      { // init
#ifdef _WIN32_WINNT
        WSADATA wsa;
        if(::WSAStartup(MAKEWORD(2,2), &wsa) != STATUS_OK)
        {
          throw core::CException(::gai_strerror(errno), __FILE__, __LINE__);
        }
#else // LINUX
        // do nothig
#endif // _WIN32_WINNT
      }

      try // listening/server socket
      {
        mSocket = new CSocket(CSocket::SERVER, mPort, mHost, mProtocol, EAddressType::IPV4);
      }
      catch (core::CException& e)
      {
        free();
        throw e;
      }
    }

    void free(void)
    {
      close();
      delete mSocket;
      
#ifdef _WIN32_WINNT
      WSACleanup();
#else // LINUX
      // do nothing
#endif
    }
    
    CMessage recv(socket_t socket) throw(CMessageException)
    {
      char     chunk[32];
      ssize_t  length = sizeof(chunk);
      result_t result = length;
      
      if(mProtocol == EProtocol::TCP)
      {
        CTcpMessage msg(socket);
        
        do
        {
          result = ::recv(socket, chunk, length, 0);
          if(result > RESULT_CLOSED)
            msg.append(chunk, result);
          if(result < length)
            break;
        }
        while(result > RESULT_CLOSED); // || errno == EINTR/WSAEINTR
        
        if(result >= RESULT_CLOSED)
          msg.status(result);
        else // if(result == RESULT_CLOSED)
          throw CMessageException(CError(), __FILE__, __LINE__);
        
        
        // @todo
        // add from address to message
        
        return msg;
      }
      else // EProtocol::UDP
      {
        CMessage msg;
      
        sockaddr_t sockaddr;
        socklen_t  addlen = sizeof(sockaddr_t);
        ::recvfrom(socket, chunk, sizeof(chunk), 0, &sockaddr, &addlen);
        
        // @todo
        // add from address to message
        
        return msg;
      }
    }
    
    size_t send(const CMessage& msg, socket_t socket)
    {
      char*    cursor = const_cast<char*>(msg.mMessage.c_str());
      ssize_t  length = msg.size();
      result_t result = length;
      
      if(mProtocol == EProtocol::TCP)
      {
        do 
        {
          result = ::send(socket, cursor, length, 0);
          if(result >= length || result <= RESULT_EMPTY)
            break;
          length -= (size_t)(result);
          cursor += result;
        }
        while(length > 0 && result > RESULT_EMPTY);
        
        // if(result > RESULT_EMPTY)
          // all ok
        // else
          // not ok
        
      }
      else // UDP
      {
        //
      }
      
      return true;
    }
    
    public:
    virtual void close()
    {
      mRunning = false;
    }
    
    virtual void listen(port_t port, const host_t& host, const std::function<void(void)>& callback) = 0;
  
    virtual void on(const label_t& label, const std::function<void(CMessage&)>& callback) = 0;
    
    virtual void on(const label_t& label, const std::function<void(CError&)>& callback) = 0;
  };

  class CUdpServer : public CServer
  {
    public:
    CUdpServer() : CServer(1337, "localhost", EProtocol::UDP)
    {
      // do nothing
    }

    public:
    void listen(port_t port, const host_t& host, const std::function<void(void)>& callback)
    {
      mPort = port;
      mHost = host;

      bind();

      callback();
      
      mRunning = true;
      
      while(mRunning)
      {
        // ...
        break;
      }
    }
  
    void on(const label_t& label, const std::function<void(CMessage&)>& callback)
    {
      mActions[label] = [&callback](CEvent& e) {
        CMessageEvent& me = dynamic_cast<CMessageEvent&>(e);
        callback(me.mMessage);
      };
    }
    
    void on(const label_t& label, const std::function<void(CError&)>& callback)
    {
      mActions[label] = [&callback](CEvent& e) {
        CErrorEvent& ee = dynamic_cast<CErrorEvent&>(e);
        callback(ee.mError);
      };
    }
  };

  class CTcpServer : public CServer
  {
    friend class CConnection;
  
    public:
    std::map<socket_t, CConnection*> mConnections;

    public:
    CTcpServer(void) : CServer(8080, "localhost", EProtocol::TCP)
    {
      // do nothing
    }
    
    ~CTcpServer()
    {
      std::cout << "> Closing all connections..." << std::endl;
      for(auto it = mConnections.begin(); it != mConnections.end(); ++it)
        delete it->second;
      mSocket->close();
    }
    
    protected:
    CConnection* connect()
    {
      socket_t newfd = ::accept(*mSocket, NULL, NULL);

      if(newfd == INVALID_SOCKET) // error new connection
        throw CConnectionException(__FILE__, __LINE__);
      
      CConnection* conn = new CConnection(newfd);
      mConnections.insert(std::make_pair(newfd, conn)); // add new connection
      
      return conn;
    }
    
    bool disconnect(socket_t socket)
    {
      auto it = mConnections.find(socket);
      if(it != mConnections.end())
      {
        it->second->close();
        delete it->second;
        mConnections.erase(it);
        return true;
      }
      return false;
    }
    
    public:
    virtual void listen(const std::function<void(void)>& callback = noop)
    {
      listen(mPort, mHost, callback);
    }

    virtual void listen(port_t port, const std::function<void(void)>& callback = noop)
    {
      listen(port, mHost, callback);
    }
    
    virtual void listen(port_t port, const host_t& host, const std::function<void(void)>& callback)
    {
      mPort = port;
      mHost = host;

      bind();

      callback();
      
      {
        CTcpMessage message;
        
        result_t  result  = STATUS_OK;
        timeval_t timeout = { 0, 000000 }; // no timeout
        
        fd_set ifdset; FD_ZERO(&ifdset); // incoming fd sockets
        fd_set rfdset; FD_ZERO(&rfdset); // reading  fd sockets
        fd_set wfdset; FD_ZERO(&wfdset); // writing  fd sockets
        fd_set efdset; FD_ZERO(&efdset); // error    fd sockets
        
        std::list<socket_t> fds;
        fds.push_back((socket_t)(*mSocket));
        
        size_t minfd = (*mSocket);
        size_t maxfd = minfd;
        
        mRunning = true;
        
        // float start = time();
        
        // register hidden events
        this->on("flush", [this, &ifdset](CConnection& conn) {
          std::cout << "> flushing..." << std::endl;
          // flush
          if(conn.mBuffer.size() != 0)
          {
            send(CMessage(conn.mBuffer), conn.mSocket);
            conn.mBuffer.clear();
          }
          // close
          // std::cout << conn.mState << std::endl;
          if(conn.mState == CConnection::DONE)   // done with flushing
          {
            conn.trigger(CCloseEvent());
            if(disconnect(conn.mSocket))         // disconnect
            {
              FD_CLR(conn.mSocket, &ifdset);
            }
          }
          else
          {
            conn.mState = CConnection::IDLE;
          }
        });
        
        while(mRunning)
        {
          rfdset = ifdset;
          wfdset = ifdset;
          efdset = ifdset;
          FD_SET(*mSocket, &rfdset); // add server to read
        //FD_SET(STDIN_FILENO, &rfdset); // not in win32
          FD_SET(*mSocket, &efdset); // add server to except
        //FD_SET(STDIN_FILENO, &efdset); // not in win32
          
          for(auto it = mConnections.begin(); it != mConnections.end(); ++it) 
          {
            CConnection*& conn = it->second;
            if(conn->mState == CConnection::CLOSED) 
            {
              if(disconnect(conn->mSocket))
              {
                FD_CLR(conn->mSocket, &ifdset);
              }
            }
          }
          
          result = ::select(maxfd + 1, &rfdset, &wfdset, &efdset, &timeout);
          if(result < SOCKET_OK)        // select error
          {
            std::cout << "> Error select: " << CError() << std::endl;
            mRunning = false;
          }
          else if(result == SOCKET_OK)  // timeout
          {
            //std::cout << "> Timeout select: " << gai_strerror(WSAGetLastError()) << std::endl;
          }
          else                          // fds found
          {
            for(socket_t currfd = minfd; currfd < maxfd + 1; ++currfd) // for each fd
            {
              if(FD_ISSET(currfd, &efdset)) // has exception
              {
                // @todo
                // report error, close connection
              
                std::cout << "> Socket " << currfd << " has an error. " << ::gai_strerror(::WSAGetLastError()) << std::endl;
                throw core::CException(::gai_strerror(::WSAGetLastError()), __FILE__, __LINE__);
              }
              
              if(FD_ISSET(currfd, &wfdset)) // ready for write
              {
                // @todo
                // mark connection response as sendable
                auto it = mConnections.find(currfd);
                if(it != mConnections.end())
                {
                  CConnection* conn = it->second;
                  if(conn->mState & CConnection::FLUSH)
                  {
                    std::cout << "> trigger:flush" << std::endl;
                    this->trigger(CFlushEvent(*conn));
                  }
                }
              }
              
              if(FD_ISSET(currfd, &rfdset)) // ready for read
              {
                if(currfd == *mSocket) // new connection
                {
                  try 
                  {
                    CConnection* conn = connect();
                    maxfd = conn->mSocket;
                    FD_SET(conn->mSocket, &ifdset);
                    std::cout << "> Connected to socket " << conn->mSocket << std::endl;
                    
                    this->trigger(CConnectionEvent(*conn));
                  } 
                  catch (CConnectionException& e) 
                  {
                    this->trigger(CErrorEvent(CError(e.what())));
                  }
                }
                else                   // incoming data
                {
                  std::cout << "> Incoming data..." << std::endl;
                  try 
                  {
                    auto it = mConnections.find(currfd);
                    if(it == mConnections.end())
                    {
                      this->trigger(CErrorEvent(CError("Failed to find a connection for this message!")));
                    }
                    else
                    {
                      CConnection* conn = it->second;
                      CMessage input = recv(currfd);
                      
                      if(input.status() == STATUS_CLOSED) // connection closed
                      {
                        std::cout << "> Received 0 bytes." << std::endl;
                        conn->trigger(CCloseEvent());
                        if(disconnect(currfd))              // disconnect
                        {
                          FD_CLR(currfd, &ifdset);
                        }
                      }
                      else                                  // processing data
                      {
                        message = CTcpMessage(conn->mSocket, input);
                        //CEventManager::trigger(this, CMessageEvent(message));
                        this->trigger(CMessageEvent(message));    // trigger message event for server
                        conn->trigger(CMessageEvent(message));    // trigger message event for socket
                      }
                    }
                  }
                  catch (CMessageException& e)
                  {
                    if(disconnect(currfd))                // disconnect
                    {
                      FD_CLR(currfd, &ifdset);
                    }
                  
                    // message error...log it
                    std::cout << "> Message error: " << e.getMessage() << std::endl;
                    //this->trigger(CErrorEvent(CError("Failed to find a connection for this message!")));
                  }
                }
              }
            }
          }
        }
      }
    }
  
    void on(const label_t& label, const std::function<void(CMessage&)>& callback)
    {
      mActions[label] = [&callback](CEvent& e) {
        // thread.lock
        CMessageEvent& me = dynamic_cast<CMessageEvent&>(e);
        callback(me.mMessage);
        // thread.unlock
      };
    }
    
    void on(const label_t& label, const std::function<void(CError&)>& callback)
    {
      mActions[label] = [&callback](CEvent& e) {
        CErrorEvent& ee = dynamic_cast<CErrorEvent&>(e);
        callback(ee.mError);
      };
    }
  
    void on(const label_t& label, const std::function<void(CConnection&)>& callback)
    {
      mActions[label] = [&callback](CEvent& e) {
        if(e.mLabel == "flush") {
          CFlushEvent& ce = dynamic_cast<CFlushEvent&>(e);
          callback(ce.mConnection);
        } else { // connection
          CConnectionEvent& ce = dynamic_cast<CConnectionEvent&>(e);
          callback(ce.mConnection);
        }
      };
    }
  };

  class CHttpServer : public CTcpServer
  {
    public:
    CHttpServer(void) : CTcpServer() { }

    virtual void listen(port_t port, const std::function<void(void)>& callback = noop)
    {
      listen(port, mHost, callback);
    }
    
    virtual void listen(port_t port, const host_t& host, const std::function<void(void)>& callback)
    {
      mPort = port;
      mHost = host;
      
      bind();
      
      callback();
      
      socket_t srvfd = (socket_t)(*mSocket);
      socket_t minfd;
      socket_t maxfd = srvfd;
      
      result_t  result  = STATUS_OK;
      timeval_t timeout = { 0, 000000 }; // no timeout
      
      fd_set ifdset; FD_ZERO(&ifdset); // incoming fd sockets
      fd_set rfdset; FD_ZERO(&rfdset); // reading  fd sockets
      fd_set wfdset; FD_ZERO(&wfdset); // writing  fd sockets
      fd_set efdset; FD_ZERO(&efdset); // error    fd sockets
      
      while(mRunning)
      {
        rfdset = ifdset;
        wfdset = ifdset;
        efdset = ifdset;
        
        result = ::select(maxfd + 1, &rfdset, &wfdset, &efdset, &timeout);
        if(result < SOCKET_OK)
        {
          // select error
        }
        else if (result == SOCKET_OK)
        {
          // timeout
        }
        else
        {
          for(socket_t curfd = minfd; curfd < maxfd; ++curfd)
          {
            if(FD_ISSET(curfd, &rfdset))
            {
              if(curfd == srvfd) // new connection
              {
                //CConnection conn = accept();
                //CEventManager::trigger(this, CConnectionEvent());
              }
              else               // incoming data
              {
                
                //CEventManager::trigger(this, CRequestEvent());
              }
              
              // close connection
              
              
            }
          }
        }
      }
    }
    
    virtual void on(const label_t& label, const std::function<void(CConnection&)>& callback) { /* hijack */ }
    
    virtual void on(const label_t& label, const std::function<void(CMessage&)>& callback) { /* hijack */ }
    
    virtual void on(const label_t& label, const std::function<void(CRequest&, CResponse&)>& callback)
    {
      mActions[label] = [&callback](CEvent& e) {
        CRequestEvent& re = dynamic_cast<CRequestEvent&>(e);
        CRequest& req = re.mRequest;
        CResponse res(req.mSocket);
        callback(req, res);
      };
    }
  };
}

#endif //__net_cserver_hpp__
