#ifndef __http_capp_hpp__
#define __http_capp_hpp__

#include <core/CApp.hpp>
#include <http.hpp>
#include <http/CServer.hpp>
#include <http/AMiddleware.hpp>

namespace http
{
  class CApp : public core::CApp
  {
    public:
    CServer& mServer;
    CRouter* mRouter;
    
    public:
    CApp(CServer& server) : mServer(server), mRouter(new CRouter(server))
    {
      log::info << "http::CApp::CApp(server)" << log::endl;
    }
    
    ~CApp()
    {
      delete mRouter;
      mRouter = nullptr;
    }
    
    public:
    CApp& use(AMiddleware& mid)
    {
      mid.use(*this);
      return *this;
    }
    
    void close()
    {
      mServer.close();
    }
    
    CApp& match(EVerb verb, const std::string& path, std::function<void(CRequest&, CResponse&)>&& callback)
    {
      // add callback to paths
      mRouter->match(verb, path, std::move(callback));
      return *this;
    }
    
    CApp& match(EVerb verb, std::function<void(CRequest&, CResponse&)>&& callback)
    {
      mRouter->match(verb, "", std::move(callback));
      return *this;
    }
    
    CApp& match(const std::string& path, std::function<void(CRequest&, CResponse&)>&& callback)
    {
      mRouter->match(EVerb::ALL, path, std::move(callback));
      return *this;
    } 
    
    CApp& match(std::function<void(CRequest&, CResponse&)>&& callback)
    {
      mRouter->match(EVerb::ALL, "", std::move(callback));
      return *this;
    }
  };
}

#endif // __http_capp_hpp__
