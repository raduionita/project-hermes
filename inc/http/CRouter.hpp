#ifndef __http_crouter_hpp__
#define __http_crouter_hpp__

#include <http.hpp>
#include <http/CServer.hpp>
#include <http/CRequest.hpp>
#include <http/CResponse.hpp>

#include <log/CLogger.hpp>

#include <string>
#include <vector>
#include <functional>
#include <tuple>

namespace http
{
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

          if(verb == req.verb() || verb == EVerb::ALL) // verb or all
          {
            if(path == req.path() || path.size() == 0) // regex or something
            {
              call(req, res);
              break;            // only one route
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

#endif // __http_crouter_hpp__
