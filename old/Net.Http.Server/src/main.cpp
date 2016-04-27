#include <iostream>

#include <net/http/CApp.hpp>

int main(int argc, char** argv)
{
  std::cout << "main(int, char**)" << std::endl;
  
  using namespace net;
  
  http::CApp app;
  
  app.on(event::ERROR, [](net::CErrorEvent& err, http::CRequest& req, http::CResponse& res) {
    std::cout << "error" << std::endl;
  });
  
  /*app.on("request", [](http::CRequest& req, http::CResponse& res) -> void {
  //  
  //});*/

  app.match("/", http::GET, [](http::CRequest& req, http::CResponse& res) {
    std::cout << "> matched..." << std::endl;
  });
  
  app.match("/info", [](http::CRequest& req, http::CResponse& res) {
    std::cout << "> matched..." << std::endl;
  });
  
  app.listen(8080, [](void) -> void {
    std::cout << "> listening..." << std::endl;
  });
  
  return 0;
}
