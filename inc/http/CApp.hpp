#ifndef __http_capp_hpp__
#define __http_capp_hpp__

#include <http.hpp>
#include <core/CApp.hpp>
#include <http/CServer.hpp>
#include <http/AMiddleware.hpp>

namespace http
{
  class CApp : public core::CApp
  {
    protected:
    CServer& mServer;
    
    public:
    CApp(CServer& server) : mServer(server)
    {
      log::info << "http::CApp::CApp(server)" << log::endl;
    }
    
    public:
    CApp& use(AMiddleware& mid)
    {
      mid.use(mServer);
      return *this;
    }
    
    void close()
    {
      mServer.close();
    }
  };
}

#endif // __http_capp_hpp__
