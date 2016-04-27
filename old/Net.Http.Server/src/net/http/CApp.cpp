#include <net/http/CApp.hpp>

#include <iostream>

namespace net { namespace http
{
  CApp::CApp(void) : core::CApp(), core::COnTickListener(), core::COnErrorListener()
  {
  
  }
  
  CApp::CApp(const http::CServer& oServer) : core::CApp(), mServer(oServer)
  {
  
  }
  
  CApp& CApp::run()
  {
    // main loop
    
    return *this;
  }
  
  CApp& CApp::on(core::CEventType& type, std::function<void(CErrorEvent&, CRequest&, CResponse&)> callback)
  {
    mCallbacks.push_back(std::make_pair([this, &callback](CEvent& event) -> void {
      CErrorEvent& errorevent = dynamic_cast<CErrorEvent&>(event);
      callback(errorevent, mServer.mRequest, mServer.mResponse);
    }, type));
    
    return *this;
  }
  
  CApp& CApp::listen(ushort port, std::function<void(void)> callback)
  {
    //on("connect", [](CEvent) -> void { callback(); });
    //mServer.connect(port);
    //run();
    
    return *this;
  }
  
  CApp& CApp::match(const std::string& pattern, http::EVerb verb, std::function<void(CRequest&, CResponse&)> callback)
  {
    // add to paths
    //callback(mServer.getRequest(), mServer.getResponse());
    
    return *this;
  }
  
  CApp& CApp::match(const std::string& pattern, std::function<void(CRequest&, CResponse&)> callback)
  {
    return match(pattern, http::GET, callback);
  }
  
  void CApp::onTick(core::CTickEvent& event)
  {
    std::cout << "CApp::onTick(CTickEvent&)" << std::endl;
  }
  
  void CApp::onError(CErrorEvent& event)
  {
    for(auto it = mCallbacks.begin(); it != mCallbacks.end(); ++it)
    {
      std::function<void(CEvent&)> callback = it->first;
      core::CEventType& type                = it->second;
      if(event.getType() == type) 
        callback(event);
    }
  }
}}
