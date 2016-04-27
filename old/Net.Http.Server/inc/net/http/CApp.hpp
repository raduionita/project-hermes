#ifndef __net_http_capp_hpp__
#define __net_http_capp_hpp__

#include <string>
#include <functional>
#include <vector>
#include <utility>

#include <core/CListener.hpp>
#include <net/http.hpp>
#include <net/CEvent.hpp>
#include <net/http/CServer.hpp>
#include <net/http/CRequest.hpp>
#include <net/http/CResponse.hpp>

namespace net { namespace http
{
  using CEventType = core::CEventType;
  using CTickEvent = core::CTickEvent;

  class CApp : public core::CApp, core::COnTickListener, COnErrorListener /*, COnRequestListener */
  {
    friend class core::CEventManager;
  
    protected:
    std::vector<std::pair<std::function<void(CEvent&)>, CEventType>> mCallbacks;
    http::CServer                                                    mServer;
    
    public:
    CApp();
    CApp(const http::CServer& mServer);
    
    public:
    CApp& run();
    CApp& listen(ushort, std::function<void(void)>);
    CApp& on(CEventType&, std::function<void(CErrorEvent&, CRequest&, CResponse&)>);
    CApp& match(const std::string&, EVerb, std::function<void(CRequest&, CResponse&)>);
    CApp& match(const std::string&, std::function<void(CRequest&, CResponse&)>);
    
    protected:
    void onTick(CTickEvent&);
    void onError(CErrorEvent&);
  };
}}

#endif // __net_http_capp_hpp__
