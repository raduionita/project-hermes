#ifndef __http__
#define __http__

#include <core/CEvent.hpp>
#include <core/CEventEmmiter.hpp>
#include <core/CSynchronizable.hpp>
#include <core/CApp.hpp>

#include <log/CLogger.hpp>

#include <net/CServer.hpp>
#include <net/CSocket.hpp>
#include <net/CMessage.hpp>
#include <net/CError.hpp>

#include <watch.hpp>

#include <functional>
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <memory>

// @todo replace ::WSAGetLastError() with a define or something...

namespace http
{
  using namespace net;

  enum EVerb
  {
    NONE    = 0x00, 
    GET     = 0x01, 
    POST    = 0x02, 
    PUT     = 0x04, 
    #undef DELETE
    DELETE  = 0x08, 
    OPTIONS = 0x10,
    HEAD    = 0x10,
    TRACE   = 0x20,
    CONNECT = 0x40,
    PATCH   = 0x80,
    ALL     = GET | POST | PUT | DELETE | OPTIONS | HEAD | TRACE | CONNECT | PATCH
  };
  
  enum EState
  {
    IDLE = 0x01,
    DONE = 0x02
  };
  
  enum EStatus
  {
    OK          = 200,
    NOCONTENT   = 204,
    MOVED       = 301,
    NOTMODIFIED = 304,
    BADREQUEST  = 400,
    FORBIDDEN   = 403,
    NOTFOUND    = 404,
    SERVERERROR = 500,
    UNAVAILABLE = 503,
    LOOP        = 508
  };
  
  enum EType
  {
    TEXT   = 0x01,
    HTML   = 0x02,
    JSON   = 0x04,
    BYTES  = 0x08,
    STREAM = BYTES,
    FORM   = 0x10
  };
  
  enum EHead
  {
    ACCEPT         = 0x01, // Accept: text/plain
    CHARSET        = 0x02, // Accept-Charset: utf-8
    ENCODING       = 0x03, // Accept-Encoding: gzip OR deflate
    CACHE          = 0x04, // Cache-Control: no-cache
    CONNECTION     = 0x05, // Connection: keep-alive OR Connection: upgrade
    COOKIE         = 0x06, // Cookie: $Version=1; Skin=new; OR Set-Cookie: UserID=JohnDoe; Max-Age=3600; Version=1
    LENGTH         = 0x07, // Content-Length: 234
    TYPE           = 0x08, // Content-Type: application/x-www-form-urlencoded
    DATE           = 0x09, // Date: Tue, 15 Nov 1994 02:32:43 GMT
    EXPECT         = 0x11, // Expect: 100-contunue
    FORWARDED      = 0x12, // Forwarded: for=192.0.2.55;proto=http;by=203.54.12.42
    FROM           = 0x13, // From: user@site.com
    HOST           = 0x14, // Host: en.wikipedia.org:80
    ORIGIN         = 0x15, // Origin: http://www.example.com
    PRAGMA         = 0x16, // Pragm: no-cache
    PROXY          = 0x17, // Proxy: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
    RANGE          = 0x18, // Range: bytes=500-999
    REFERER        = 0x19, // Referer: http://en.wikipedia.org/wiki/Main_Page
    USERAGENT      = 0x20, // User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0
    UPGRADE        = 0x21, // Upgrade: HTTP/2.0, SHTTP/1.3, IRC/6.9, RTA/x11
    
    ETAG           = 0x10, // ETag: 737060cd8c284d8af7ad3082f209582d
    AGE            = 0x22, // Age: 12
    EXPIRES        = 0x23, // Expires: Thu, 01 Dec 1994 16:00:00 GMT
    MODIFIED       = 0x24, // Last-Modified: Tue, 15 Nov 1994 12:45:26 GMT
    SERVER         = 0x25, // Server: Apache/2.4.1 (Unix)
    STATUS         = 0x25, // Status: 200 OK
    
    CSRF           = 0x60  // X-Csrf-Token: i8XNjC4b8KVok4uw5RftR38Wgp2BFwql
  };
  
  class CRequest
  {  
    typedef const std::string param_t;
    typedef       std::string value_t;
    
    protected:
    socket_t                       mSocket;
    watch::milliseconds            mTime;
    EVerb                          mVerb;
    std::string                    mPath;
    std::map<param_t, value_t>     mParams;
    
    std::map<param_t, value_t>     mHeads;
    std::string                    mBody;
    
    public:
    CRequest(CSocket& sock)
    : mSocket(sock), mTime(watch::millitime()), mVerb(EVerb::ALL), mPath("/")
    {
      log::info << "http::CRequest::CRequest()" << log::endl;
      // @todo Fill with defaults
    }
    
    CRequest(CSocket& sock, const CMessage& msg)
    : mSocket(sock), mTime(watch::millitime()), mVerb(EVerb::ALL), mPath("/"), mBody(msg.mMessage)
    {
      log::info << "http::CRequest::CRequest(sock, msg)" << log::endl;
      // @todo Build header & body from CMessage
    }
    
    CRequest(CRequest&& that) 
    : mSocket(that.mSocket)
    {
      log::info << "http::CRequest::CRequest(that&&)" << log::endl;
      mVerb   = std::move(that.mVerb);
      mPath   = std::move(that.mPath);
      mParams = std::move(that.mParams);
      mHeads  = std::move(that.mHeads);
      mBody   = std::move(that.mBody);
      mTime   = std::move(that.mTime);
    }
    
    virtual ~CRequest()
    {
      
    }
    
    public:
    socket_t socket() const
    {
      return mSocket;
    }
    
    EVerb verb() const
    {
      return mVerb;
    }
    
    std::string path() const
    {
      return mPath;
    }
    
    value_t param(const param_t& key) const
    {
      auto it = mParams.find(key);
      if(it == mParams.end())
        return "undefined";
      return it->second;
    }
  
    value_t head(const param_t& key) const
    {
      auto it = mHeads.find(key);
      if(it == mHeads.end())
        return "undefined";
      return it->second;
    }
  
    std::string body() const
    {
      return mBody;
    }
  
    watch::milliseconds time() const
    {
      return mTime;
    }
  };
  
  class CResponse : public core::CSynchronizable
  {
    protected:
    socket_t                       mSocket;
    watch::milliseconds            mTime;
    std::map<EHead, std::string>   mHead;
    std::string                    mBody;
    uint                           mStatus;
    EState                         mState;
  
    public:
    CResponse(CSocket& sock) 
    : mSocket(sock), mTime(watch::millitime())
    {
      log::info << "http::CResponse::CResponse(sock)" << log::endl;
      mStatus            = http::OK;
      mState             = EState::IDLE;
      mHead[EHead::TYPE] = "text/plain";
    }
    
    CResponse(CResponse&& that)
    : mSocket(that.mSocket)
    {
      log::info << "http::CResponse::CResponse(that&&)" << log::endl;
      mHead   = std::move(that.mHead);
      mBody   = std::move(that.mBody);
      mStatus = std::move(that.mStatus);
      mState  = std::move(that.mState);
      mTime   = std::move(that.mTime);
    }
    
    public:
    socket_t socket() const
    {
      return mSocket;
    }
    
    bool is(EState state) const
    {
      return mState == state;
    }
    
    EState state() const
    {
      return mState;
    }
    
    CResponse& status(EStatus val)
    {
      mStatus = val;
      mHead[EHead::STATUS] = (int)(val);
      return *this;
    }
    
    CResponse& head(EHead key, const std::string& value)
    {
      mHead[key] = value;
      // @todo Some processing may be needed
      return *this;
    }
    
    std::string head(EHead key) const
    {
      auto it = mHead.find(key);
      if(it == mHead.end())
        return "undefined";
      return it->second;
    }
    
    CResponse& type(EType val)
    {
      std::string res("text/plain");
      
      // @todo Need more types...
      
      if(val == EType::HTML)
        res = "text/html";
      else if(val == EType::JSON)
        res = "application/json";
      else if(val == EType::BYTES || val == EType::STREAM)
        res = "application/octet-stream";
      else if(val == EType::FORM)
        res = "multipart/form-data";
      
      head(EHead::TYPE, res);
      return *this;
    }
    
    std::string type() const
    {
      return head(EHead::TYPE);
    }
    
    CResponse& write(const std::string& data)
    {
      mBody.append(data);
      mState = EState::IDLE;
      return *this;
    }
    
    std::string read(size_t length)
    {
      std::string chunk = mBody.substr(0, length);
      mBody.erase(length);
      return chunk;
    }
    
    CResponse& end(const std::string& data)
    {
      mBody.append(data);
      mState = EState::DONE;
      return *this;
    }
    
    CResponse& end()
    {
      mState = EState::DONE;
      return *this;
    }
    
    watch::milliseconds time() const
    {
      return mTime;
    }
  };

  class CRequestEvent : public CEvent
  {
    typedef std::shared_ptr<CRequest>  request_t;
    typedef std::shared_ptr<CResponse> response_t;
  
    public:
    request_t mRequest;
    
    public:
    CRequestEvent(request_t request) : CEvent("request"), mRequest(request)
    {
      log::info << "http::CRequestEvent::CRequestEvent(request)" << log::endl;
    }
  };
  
  class CServer : public net::AServer, public CEventEmmiter
  {
    typedef std::shared_ptr<CRequest>        request_t;
    typedef std::shared_ptr<CResponse>       response_t;
    typedef std::pair<request_t, response_t> client_t;
    typedef net::CServer self;
    typedef net::AServer parent;
    
    protected:
    net::CSocket*                mServer;  // ptr::shared<CSocket> mServer;
    std::map<socket_t, client_t> mClients; // @todo Need shared pointers
    
    public:
    port_t         mPort;    // 80, 8080
    host_t         mHost;    // NULL, localhost, 127.0.0.1 or sub.domain.tld
    watch::seconds mTime;    // seconds
    bool           mRunning;
    
    public:
    CServer() 
    : net::AServer(), mServer(nullptr), mPort(8080), mHost("localhost"), mTime(10), mRunning(false)
    {
      log::info << "net::CServer::CServer()" << log::endl;
    }
    
    CServer(port_t port, host_t host) 
    : net::AServer(), mServer(nullptr), mPort(port), mHost(host), mTime(10), mRunning(false)
    {
      log::info << "net::CServer::CServer(port, host)" << log::endl;
    }
    
    ~CServer()
    {
      log::info << "net::CServer::~CServer()" << log::endl;
      
      while(mClients.size() > 0) // safely close client connections
      {
        log::info << "> Clients pending: " << mClients.size() << log::endl;
      
        auto then = watch::millitime();
        then -= mTime;
        bool wait = false;
        
        for(auto it = mClients.begin(); it != mClients.end();/* ++it */) 
        {
          if(it->second.first == nullptr || it->second.second == nullptr)
          {
            ::close(it->first);
            mClients.erase(it++);
          }
          else
          {
            if(it->second.second->is(EState::DONE))       // ready for erase
            {
              log::info << "> Removing client... " << log::endl;
            
              // /* CResponse* */     delete it->second.second; it->second.second = nullptr;
              // /* CRequest* */      delete it->second.first;  it->second.first  = nullptr;
              ::close(it->first);
              
              mClients.erase(it++);
            }
            else
            {
              log::info << "> Time: " << it->second.second->time()  << " < " << then << log::endl;
              
              if(it->second.second->time() < then) // out of time
              {
                log::info << "> Ending client..." << log::endl;
                
                it->second.second->end();
                
                // @todo Try to send the response before closing
              }
              else                                           // wait another second
              {
                log::info << "> Not ready yet, I should wait..." << log::endl;
                wait = true;
              }
              ++it;
            }
          }
        }
        
        if(wait) // does it still need to wait?
        {
          log::info << "> Sleeping for 1 sec... " << log::endl;
          std::this_thread::sleep_for(watch::seconds(1));
        }
      }
      
      mServer->close();
      delete mServer; mServer = nullptr;
    }
  
    public:
    void bind()
    {
      log::info << "http::CServer::bind()" << log::endl;
      
      try // listening/server socket
      {
        mServer = new net::CSocket(net::CSocket::SERVER, mPort, NULL, EProtocol::TCP, EAddressType::IPV4);
      }
      catch (core::CException& e)
      {
        close();
        throw e;
      }
    }
    
    void close()
    { 
      log::info << "http::CServer::close()" << log::endl;
      mRunning = false;
    }
    
    CServer& listen(std::function<void(void)> callback = core::noop)
    {
      log::info << "http::CServer::listen(callback)" << log::endl;
    
      bind();     // server socket created
    
      callback(); // ready for loop
    
      result_t  result = STATUS_OK;
      timeval_t timeout = { 0, 000000 }; // no timeout
      
      fd_set ifdset; FD_ZERO(&ifdset); // incoming fd sockets
      fd_set rfdset; FD_ZERO(&rfdset); // reading  fd sockets
      fd_set wfdset; FD_ZERO(&wfdset); // writing  fd sockets
      fd_set efdset; FD_ZERO(&efdset); // error    fd sockets
      
      socket_t srvfd = (socket_t)(*mServer);
      socket_t minfd = srvfd;
      socket_t maxfd = minfd;
      
      mRunning = true;
      while(mRunning)
      {
        rfdset = ifdset;
        wfdset = ifdset;
        efdset = ifdset;
        FD_SET(srvfd, &rfdset); // add server to read
      //FD_SET(STDIN_FILENO, &rfdset); // not in win32
        FD_SET(srvfd, &efdset); // add server to except
      //FD_SET(STDIN_FILENO, &efdset); // not in win32
      
        // @todo Normalize maxfd
        // @todo Check for unclosed clients
        
        result = ::select(maxfd + 1, &rfdset, &wfdset, &efdset, &timeout);
        if(result < RESULT_OK)       // error
        {
          // error
          log::error << "> Error select() " << log::endl;
          //CEventEmmiter::emit(new CErrorEvent(error));
          close();
        }
        else if(result == RESULT_OK) // timeout
        {
          // timeout
          log::info << log::spinner; // log::warn << "> Timeout! " << log::endl;
        }
        else                         // result > RESULT_OK, result = num_sockets
        {
          // read/write...
          for(socket_t curfd = minfd; curfd < (maxfd + 1); ++curfd)
          {
            if(FD_ISSET(curfd, &efdset)) // has exception
            {
              // @todo Close client/response
              log::error << "> Exception on socket " << curfd << log::endl;
              // @todo Remove from maxfd if you can
              FD_CLR(curfd, &ifdset);
              continue;
            }
            
            if(FD_ISSET(curfd, &wfdset)) // can/ready for write
            {
              // @todo Mark writable
              log::info << log::spinner;
              // log::info << "Socket " << curfd << " is writable" << log::endl;
            }
            
            if(FD_ISSET(curfd, &rfdset)) // ready for read
            {
              if(curfd == srvfd) // new connection
              {
                // @todo Move this to a method and surrounde it with a try catch
                socket_t newfd = ::accept(srvfd, NULL, NULL);
                if(newfd == INVALID_SOCKET)
                {
                  log::error << "Connection error!" << log::endl;
                  // @todo emit -> new CErrorEvent(error)
                }
                else
                {
                  minfd = std::min(newfd, minfd);
                  maxfd = std::max(newfd, maxfd);
                  
                  FD_SET(newfd, &ifdset);
                  mClients[newfd]; // touch
                  // mClients.insert(std::make_pair(newfd, std::make_pair(nullptr, nullptr)));
                  //CEventEmmiter::emit(new CConnectionEvent(con));
                }
              }
              else                // incoming data
              {
                log::info << "> Incoming message from socket " << curfd << "." << log::endl;
                
                std::string buffer;
                char        chunk[4096 + 1];
                ssize_t     length = sizeof(chunk) - 1; // 33 - 1
                while(true)
                {
                  result = ::recv(curfd, chunk, length, 0);
                  
                  if(result < 0)        // error
                  {
                    if(error() != net::CError::WOULDBLOCK && error() != net::CError::AGAIN)
                    {
                      log::error << "> Error receiving data for socket " << curfd << "." << log::endl;
                      log::error << error() << log::endl;
                      log::warn << "> Closing connection on socket " << curfd << "." << log::endl;
                      FD_CLR(curfd, &ifdset);
                      ::close(curfd);
                      mClients.erase(curfd);

                      // @todo remove client from mClients
                      // @todo throw exception
                      // @todo emit -> new CErrorEvent(error)
                      // @todo emit -> new CCloseEvent()
                    }
                    break;
                  }
                  else if(result == 0) // close connection
                  {
                    log::warn << "> Closing connection on socket " << curfd << "." << log::endl;
                    FD_CLR(curfd, &ifdset);
                    ::close(curfd);
                    mClients.erase(curfd);
                    // @todo remove client from mClients
                    // @todo emit -> new CCloseEvent()
                    break;
                  }
                  else                 // chunk complete
                  {
                    chunk[result] = '\0';
                    buffer.append(chunk);
                    if(result < length || result > length)
                    {
                      break;
                      // @todo build request
                      // @todo emit -> new CRequestEvent(request)
                    }
                  }
                }
                
                log::info << buffer << log::endl;
                
                char resp[] = "HTTP/1.1 200 OK";
                result = ::send(curfd, resp, sizeof(resp), 0);
                if(result < 0)
                {
                  log::error << "> Error sending data for socket " << curfd << "." << log::endl;
                  log::error << ::gai_strerror(::WSAGetLastError()) << log::endl;
                }
                else 
                {
                  log::info << "> Response sent on socket " << curfd << "." << log::endl;
                }
                
                FD_CLR(curfd, &ifdset);
                ::close(curfd);
                mClients.erase(curfd);
              }
            }
          }
        }
        
        
        
        
        
        
        
        
        
        
        
        
        
//        {
//          CSocket*  sock = new CSocket(CSocket::INVALID);
//          CMessage* msg  = new CMessage("request test");
//          auto out = mClients.insert(std::make_pair(sock, std::make_pair(std::make_shared<CRequest>(*sock, *msg), new CResponse(*sock))));
//          CEventEmmiter::emit(new CRequestEvent(*(out.first->second.first)));
//          CEventEmmiter::emit(new CRequestEvent(*(out.first->second.first)));
//          
//          delete msg;
//        }
      }
      
      return *this;
    }
    
    CServer& on(const label_t& label, std::function<void(CRequest&, CResponse&)>&& callback)
    {
      log::info << "http::CServer::on('" << label << "', callback)" << log::endl;
      
      CEventEmmiter::add(label, [this, callback](const CEvent* e) {
        const CRequestEvent* re = dynamic_cast<const CRequestEvent*>(e);
        const request_t&  req = re->mRequest;
        const response_t& res = mClients[req->socket()].second;
        
        synchronized(*res) // prevent threads with same response from overwriting each other
        {
          callback(*req, *res);
        }
      });
      
      return *this;
    }
  };

  class CRouter
  {
    typedef std::string                                path_t;
    typedef std::function<void(CRequest&, CResponse&)> callback_t;
    typedef std::tuple<EVerb, path_t, callback_t>      route_t;
    
    protected:
    CServer&             mServer;
    std::vector<route_t> mRoutes;
    
    public:
    CRouter(CServer& server) : mServer(server)
    {
      log::info << "http::CRouter::CRouter(server)" << log::endl;
      init();
    }
    
    ~CRouter()
    {
      log::info << "http::CRouter::~CRouter()" << log::endl;
    }
    
    protected:
    void init()
    {
      mServer.on("request", [this](http::CRequest& req, http::CResponse& res) {
        // go through all mRoutes...see what matches...run its callback
        for(auto it = mRoutes.begin(); it != mRoutes.end(); ++it) 
        {
          EVerb              verb = std::get<0>(*it);
          const std::string& path = std::get<1>(*it);
          const callback_t&  call = std::get<2>(*it);

          if(verb == req.verb() || verb == EVerb::ALL) //
          {
            if(path == req.path() || path.size() == 0) // regex or something
            {
              call(req, res);
            }
          }
        }
      });
    }
    
    public:
    CRouter& match(EVerb verb, const std::string& path, std::function<void(CRequest&, CResponse&)>&& callback)
    {
      // add callback to paths
      mRoutes.push_back(std::make_tuple(verb, path, callback));
      return *this;
    }
    
    CRouter& match(EVerb verb, std::function<void(CRequest&, CResponse&)>&& callback)
    {
      mRoutes.push_back(std::make_tuple(verb, "", callback));
      return *this;
    }
    
    CRouter& match(const std::string& path, std::function<void(CRequest&, CResponse&)>&& callback)
    {
      mRoutes.push_back(std::make_tuple(EVerb::ALL, path, callback));
      return *this;
    }
    
    CRouter& match(std::function<void(CRequest&, CResponse&)>&& callback)
    {
      mRoutes.push_back(std::make_tuple(EVerb::ALL, "", callback));
      return *this;
    }
  };

  class CApp : public core::CApp
  {
    protected:
    CServer& mServer;
    
    public:
    CApp(CServer& server) : mServer(server)
    {
      log::info << "http::CApp::CApp(server)" << log::endl;
    }
    
    void close()
    {
      mServer.close();
    }
  };
}

#endif // __http__
