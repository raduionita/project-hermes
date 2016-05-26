#include <http/CServer.hpp>
#include <os.hpp>
#include <watch.hpp>

#include <thread>
#include <sstream>

int main(int argc, char** argv)
{
  log::info << "::main(argc, argv)" << log::endl;
  
  http::CServer  server;
  http::CRouter  router(server);
  http::CApp     app(server);
  router.match(http::GET, "/favicon.ico", [](http::CRequest& req, http::CResponse& res) {
    log::info << "> /favicon.ico" << log::endl;
    log::info << "> " << req.head(http::HOST) << log::endl;
    res.type(http::ICON);
    res.end();
  });
  router.match(http::GET, [](http::CRequest& req, http::CResponse& res) {
    log::info << "> " << req.path() << log::endl;
    log::info << "> " << req.head(http::HOST) << log::endl;
    res.type(http::HTML);
    std::stringstream output;
    output << "<html><head><title>It works!</title></head><body><h3>It works!</h3><code>" << req.head(http::USERAGENT) << "</code></body>";
    res.send(output.str());
    res.end();
  });
  server.listen([]() {
    log::info << "> Listening." << log::endl;
  });
  
  return 0;
}

/*
// restfull
  // RESOURCE
    // /tickets              # get all tickets
    // /tickets/19           # get a ticket 
    // /tickets/10/messages  # get all messages within tickets
  // FILTERING
    // /tickets?order=-priority                                           # order by priority desc
    // /tickets?filter[status]=1                                          # where status = 1
    // /tickets?fields=id,subject,status&order=-updated&limit=10          # only specific fields
    // /tickets?filter[subject]=%something                                # where subject like '%something'

    
// www.exmaple.com/my-cool-product-category 
// api.example.com/products/413
*/
