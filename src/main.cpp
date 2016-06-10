#include <http/CServer.hpp>
#include <http/CRouter.hpp>
#include <http/CApp.hpp>
#include <http/CFileSystem.hpp>

#include <sstream>
#include <fstream>

#include <dirent.h>
#include <sys/stat.h>

int main(int argc, char** argv)
{
  log::info << "::main(argc, argv)" << log::endl;  
  
  http::CServer      srv;          // http::server()
  http::CApp         app(srv);     // for handling system events
  http::CFileSystem  filesystem("public");
  // http::CCaching     cache("cache");
  // http::CTplRenderer tpl("views");
  // os::CFileIO        fio;
  
  app.use(filesystem);
  // app.use(cache);
  // tpl.render("path/to/file.tpl", data);
  // srv.on("close", ...);
  
  app.match(http::GET, "/products/{id}", [](http::CRequest& req, http::CResponse& res) {
    log::info << "> " << req.head(http::HOST) << " " << req.path() << " " << req.param("id") << log::endl;
    res.status(http::OK);
    res.send("test");
    res.end();
  });
  app.match(http::GET, "*", [](http::CRequest& req, http::CResponse& res) {
    res.type(http::HTML);
    
    std::ifstream ifs("views/index.html", std::ios::binary | std::ios::in);
    if(ifs) 
    {
      std::stringstream ss;
      ss << ifs.rdbuf();
      res.status(http::OK);
      res.send(ss.str());
    }
    else
    {
      res.status(http::NOTFOUND);
      log::warn << "> Cannot open file!" << log::endl;
    }
    
    //res.send(fio.read("relative/path/to/file", file::UTF8));
    //res.send(tpl.render("relative/path/to/template.tpl", data));
    
    res.end();
  });
  srv.listen([]() {
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
