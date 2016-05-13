#define LOGTIME 0

#include <http.hpp>

#include <thread>
#include <chrono>

int main(int argc, char** argv)
{
  log::info << "::main(argc, argv) > " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << log::endl;
  
  http::CServer server;
  http::CRouter router(server);
  router.match(http::ALL, [](http::CRequest& req, http::CResponse& res) {
    res.type(http::JSON);
    log::info << "> Router match" << log::endl;
  });
  server.on("request", [](http::CRequest& req, http::CResponse& res) {
    //std::this_thread::sleep_for(std::chrono::seconds(2));
    log::info << ("> Server request: ") << req.body() << " Response type: " << res.type() << log::endl;
  });
  server.listen([]() {
    log::info << "> Listening." << log::endl;
  });
  return 0;
}

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
