#ifndef __http__
#define __http__

#include <core/CEvent.hpp>
#include <core/CEventEmmiter.hpp>
#include <core/CSynchronizable.hpp>

#include <log/CLogger.hpp>

#include <net/CServer.hpp>
#include <net/CSocket.hpp>
#include <net/CMessage.hpp>

#include <functional>
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <chrono>

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
    std::chrono::milliseconds::rep mTimestamp;
    CSocket&                       mSocket;
    EVerb                          mVerb;
    std::string                    mPath;
    std::map<param_t, value_t>     mParams;
    
    std::map<param_t, value_t>     mHeads;
    std::string                    mBody;
    
    public:
    CRequest(CSocket& sock)
    : mSocket(sock), mVerb(EVerb::ALL), mPath("/")
    {
      log::info << "http::CRequest::CRequest()" << log::endl;
      // @todo: fill with defaults
      
      mTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    CRequest(CSocket& sock, const CMessage& msg)
    : mSocket(sock), mVerb(EVerb::ALL), mPath("/"), mBody(msg.mMessage)
    {
      // @todo: build header & body from CMessage
      log::info << "http::CRequest::CRequest(sock, msg)" << log::endl;
      
      mTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    CRequest(CRequest&& that) 
    : mSocket(that.mSocket)
    {
      log::info << "http::CRequest::CRequest(that&&)" << log::endl;
      mVerb      = std::move(that.mVerb);
      mPath      = std::move(that.mPath);
      mParams    = std::move(that.mParams);
      mHeads     = std::move(that.mHeads);
      mBody      = std::move(that.mBody);
      mTimestamp = std::move(that.mTimestamp);
    }
    
    virtual ~CRequest()
    {
      
    }
    
    public:
    CSocket& socket() const
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
  
    std::chrono::milliseconds::rep timestamp() const
    {
      return mTimestamp;
    }
  };
  
  class CResponse : public core::CSynchronizable
  {
    public:
    enum EState
    {
      IDLE = 0x01,
      DONE = 0x02
    };
    
    protected:
    std::chrono::milliseconds::rep mTimestamp;
    CSocket&                       mSocket;
    std::map<EHead, std::string>   mHead;
    std::string                    mBody;
    uint                           mStatus;
    EState                         mState;
  
    public:
    CResponse(CSocket& sock) 
    : mSocket(sock)
    {
      log::info << "http::CResponse::CResponse(sock)" << log::endl;
      mStatus            = http::OK;
      mState             = EState::IDLE;
      mHead[EHead::TYPE] = "text/plain";
      mTimestamp         = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    CResponse(CResponse&& that)
    : mSocket(that.mSocket)
    {
      log::info << "http::CResponse::CResponse(that&&)" << log::endl;
      mSocket  = std::move(that.mSocket);
      mHead    = std::move(that.mHead);
      mBody    = std::move(that.mBody);
      mStatus  = std::move(that.mStatus);
      mState   = std::move(that.mState);
    }
    
    public:
    CSocket& socket() const
    {
      return mSocket;
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
    
    std::chrono::milliseconds::rep timestamp() const
    {
      return mTimestamp;
    }
  };

  class CRequestEvent : public CEvent
  {
    public:
    CRequest& mRequest;
    
    public:
    CRequestEvent(CRequest& request) : CEvent("request"), mRequest(request)
    {
      log::info << "http::CRequestEvent::CRequestEvent(request)" << log::endl;
    }
  };
  
  class CServer : public net::AServer, public CEventEmmiter
  {
    protected:
    net::CSocket*                                        mServer;
    std::map<CSocket*, std::pair<CRequest*, CResponse*>> mClients;
    
    public:
    port_t mPort;
    host_t mHost;
    
    public:
    CServer() 
    : net::AServer(), mServer(nullptr), mPort(8080), mHost("localhost")
    {
      log::info << "net::CServer::CServer()" << log::endl;
    }
    
    CServer(port_t port, host_t host) 
    : net::AServer(), mServer(nullptr), mPort(port), mHost(host)
    {
      log::info << "net::CServer::CServer(port, host)" << log::endl;
    }
    
    ~CServer()
    {
      // @todo Wait for all CResponse's to be in CResponse::DONE state
      
      log::info << "net::CServer::~CServer()" << log::endl;
      for(auto it = mClients.begin(); it != mClients.end(); ++it) 
      {
        /* CResponse* */     delete it->second.second;  // it->second.second = nullptr;
        /* CRequest* */      delete it->second.first;   // it->second.first = nullptr;
        it->first->close();
        /* const CSocket* */ delete it->first;          // it->first = nullptr;
      }
      close();
      delete mServer;
    }
  
    public:
    void bind()
    {
      log::info << "http::CServer::bind()" << log::endl;
      
      try // listening/server socket
      {
        mServer = new net::CSocket(net::CSocket::SERVER, mPort, mHost, EProtocol::TCP, EAddressType::IPV4);
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
      mServer->close();
    }
    
    CServer& listen(std::function<void(void)> callback = core::noop)
    {
      log::info << "http::CServer::listen(callback)" << log::endl;
    
      bind();
    
      callback();
    
      while(true)
      {
        // trigger events...
        {
          CSocket*  sock = new CSocket(CSocket::INVALID);
          CMessage* msg  = new CMessage("request 0");
          auto out = mClients.insert(std::move(std::make_pair(std::move(sock), std::move(std::make_pair(std::move(new CRequest(*sock, *msg)), std::move(new CResponse(*sock)))))));
          CEventEmmiter::emit(new CRequestEvent(*(out.first->second.first)));
          CEventEmmiter::emit(new CRequestEvent(*(out.first->second.first)));
          CEventEmmiter::emit(new CRequestEvent(*(out.first->second.first)));
          
          delete msg;
        }

        {
          CSocket*  sock = new CSocket(CSocket::INVALID);
          CMessage* msg  = new CMessage("request 1");
          auto out = mClients.insert(std::move(std::make_pair(std::move(sock), std::move(std::make_pair(std::move(new CRequest(*sock, *msg)), std::move(new CResponse(*sock)))))));
          CEventEmmiter::emit(new CRequestEvent(*(out.first->second.first)));
          CEventEmmiter::emit(new CRequestEvent(*(out.first->second.first))); 
          
          delete msg;
        }
        
        break;
      }
      
      // @todo Check all CResponse's
      
      std::this_thread::sleep_for(std::chrono::seconds(10));
      
      return *this;
    }
    
    CServer& on(const label_t& label, std::function<void(CRequest&, CResponse&)>&& callback)
    {
      log::info << "http::CServer::on('" << label << "', callback)" << log::endl;
      
      CEventEmmiter::add(label, [this, callback](const CEvent* e) {
        const CRequestEvent* re = dynamic_cast<const CRequestEvent*>(e);
        CRequest*  req = &(re->mRequest);
        CResponse* res = mClients[&(req->socket())].second;
        
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
}

#endif // __http__
