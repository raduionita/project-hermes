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
  
  http::CServer     server;          // http::server()
  http::CRouter     router(server);  // http::route(server)
  http::CApp        app(server);     // for handling system events
    
  // http::CServer      srv;
  // http::CApp         app(srv);
  http::CFileSystem  filesystem("public");
  // http::CCaching     cache;
  // http::CTplRenderer tpl("views");
  // os::CFileIO        fio;
  
  // app.use(cache);
  app.use(filesystem);
  // app.match(http::GET, "/path", [](http::CRequest& req, http::CResponse& res) { });
  // tpl.render("path/to/file.tpl", data);
  // srv.on("close", ...);
  
  router.match(http::GET, "/favicon.ico", [](http::CRequest& req, http::CResponse& res) {
    log::info << "> " << req.head(http::HOST) << " /favicon.ico" << log::endl;
    
    res.type(http::ICON);
    
    std::ifstream ifs("favicon.ico", std::ios::binary | std::ios::in);
    if(ifs) 
    {
      std::string output;
      ifs.seekg(0, std::ios::end);
      //output.reserve(ifs.tellg());
      ifs.seekg(0, std::ios::beg);
      //output.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    
      res.status(http::OK);
      res.send(output);
    }
    else
    {
      res.status(http::NOTFOUND);
      log::warn << "> Cannot open file!" << log::endl;
    }
    
    res.end();
  });  
  router.match(http::GET, "/products/:id", [](http::CRequest& req, http::CResponse& res) {
    log::info << "> " << req.head(http::HOST) << " " << req.path() << log::endl;
    
  });
  router.match(http::GET, [](http::CRequest& req, http::CResponse& res) {
    log::info << "> " << req.head(http::HOST) << " " << req.path()  << log::endl;
    
    res.type(http::HTML);
    std::ifstream ifs("index.html", std::ios::binary | std::ios::in);
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
    
    res.end();
    
    //res.send(fio.read("relative/path/to/file", file::UTF8));
    //res.send(tpl.render("relative/path/to/template.tpl", data));
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
