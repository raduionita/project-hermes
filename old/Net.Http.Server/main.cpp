namespace core
{
  class CEventListener;
  
  class CApp;
  
  class CNetApp : public CApp;
}

namespace net
{
  class CServer : public core::CEventListener
  {
    CServer& listen(port = randport(), host = 'localhost', callback = noop);
    CServer& listen(port, callback = noop);
    CServer& listen(callback);
    
    CServer& on(event, callback);
  };
  
  class CClient
  {
    
  };
  
  class CResponse
  {
    
  };
  
  class CRequest
  {
    
  };
  
  //////////////////////////////////////////
  
  class CHttpServer : public CServer
  {
    std::map<verb, std::map<std::string, std::function>> mPaths;
    
    CHttpServer& listen()
    {
      while(true)
      {
        
      }
    }
  };
}

int main(int argc, char** argv)
{
  // init
  net::CHttpServer http;
  
  // error
  http.on('error', [](err, req, res) -> void {
    // trigger is path not found/match
  });
  
  // request
  http.on('request', [] (req, res) -> void { // for all requests
    
  });
  
  http.on(net::verb::ALL, [](req, res) -> void { // low priority
    
  });
  
  http.on(net::verb::GET, '/', [](req, res) -> void {
    
  });
  
  http.on(net::verb::GET, '/promotion/:id', [](req, res) -> void {
    
  });
  
  // connect
  http.listen(8080, []() -> void {  
    std::cout << "Listening..." << std::endl;
  });
  
  return 0;
}