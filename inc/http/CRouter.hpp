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
    typedef std::string                                param_t;
    typedef std::string                                value_t;
    typedef std::string                                path_t;
    typedef std::function<void(CRequest&, CResponse&)> callback_t;
    typedef std::tuple<EVerb, path_t, callback_t>      route_t;
    
    protected:
    CServer&             mServer;
    std::vector<route_t> mRoutes;
    bool                 mInited;
    
    public:
    CRouter(CServer& server) : mServer(server), mInited(false)
    {
      log::info << "http::CRouter::CRouter(server)" << log::endl;
    }
    
    ~CRouter()
    {
      log::info << "http::CRouter::~CRouter()" << log::endl;
    }
    
    protected:
    void init()
    {
      log::info << "http::CRouter::init()" << log::endl;
      
      mServer.on("request", [this](http::CRequest& req, http::CResponse& res) {
        // go through all mRoutes...see what matches...run its callback
        log::info << "> CRouter ... CServer::on(request, callback)" << log::endl;
        for(auto it = mRoutes.begin(); it != mRoutes.end(); ++it) 
        {
          EVerb              verb = std::get<0>(*it);
          const std::string& path = std::get<1>(*it);
          const callback_t&  call = std::get<2>(*it);
          
          if(verb == req.verb() || verb == EVerb::ALL) // verb or all
          {
            if(path == req.path() || path == "*" || parse(path, req)) // regex or something
            {
              log::info << "> Maching... Verb: " << http::getVerb(verb) << " Path: " << path << log::endl;
              call(req, res);
              break;            // only one route
            }
          }
          
          log::info << "> [" << http::getVerb(verb) << "]" << path << " [skipped] " << log::endl;
        }
      });
    }
    
    bool parse(const path_t& path, http::CRequest& req)
    {
      const path_t& p = path;
      const path_t& r = req.path();
      
      param_t param;
      value_t value;
      
      for(size_t i = 1, j = 1, k = 0; i < p.size(); ) // ignore first /
      {
        if(p[i] == '{') // /user/{name}/action
        {
          // @todo What if param.size() == 0
        
          for(k = i+1; k < p.size(); ++k) // find param_t
          {
            if(p[k] == '}')
              break;
          }
          
          char ch = k < p.size() ? p[k+1] : '\0'; // mark char after }
          
          param = p.substr(i+1, k-(i+1));
          
          i = k; // i = pos of last '/'
          
          // @todo What if value.size() == 0
          
          for(k = j; k < r.size(); ++k) // (j-1)+1 ... at least 1 char
          {
            if(r[k] == ch) // /user/Danny/action
              break;
          }
          
          value = r.substr(j, k-j);
          
          req.param(param, value);
          
          j = k-1;  // j = pos of last '/'
        } 
        else if(p[i] != r[j]) // paths must match
        {
          return false;
        }

        ++i; ++j;
      }
      
      return true;
    }
    
    public:
    CRouter& match(EVerb verb, const path_t& path, std::function<void(CRequest&, CResponse&)>&& callback)
    {
      if(!mInited)
        init();
      // add callback to paths
      mRoutes.push_back(std::make_tuple(verb, path, callback));
      return *this;
    }
  };
}

#endif // __http_crouter_hpp__
