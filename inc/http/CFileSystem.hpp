#ifndef __http_cfilesystem_hpp__
#define __http_cfilesystem_hpp__

#include <http.hpp>
#include <http/AMiddleware.hpp>
#include <http/CServer.hpp>

namespace http
{
  class CFileSystem : public AMiddleware
  {
    public:
    CFileSystem()
    {
      // @todo Some settings...
      // buffersize
    }
  
    public:
    void use(CServer& server)
    {
      server.on("request", [this](http::CRequest& req, http::CResponse& res) {
        // trim / and dots(..)
        // try to open file
        // if fail...404
        
        // read chunks of file in a loop
        // while(still data to read)
        //   if(res.size() == full)
        //     continue;
        //   res.send(chunk)
        // res.end();
        
        // keep max 256 files open
        
        // cache(w/ timeout) small files < 1M!?
      });
    }
  };
}

#endif // __http_cfilesystem_hpp__
