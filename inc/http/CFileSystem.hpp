#ifndef __http_cfilesystem_hpp__
#define __http_cfilesystem_hpp__

#include <http.hpp>
#include <http/AMiddleware.hpp>
#include <http/CServer.hpp>
#include <os.hpp>
#include <log/CLogger.hpp>

#ifdef _WIN32_WINNT
  #define DS "\\"
#else // LINUX
  #define DS "/"
#endif // _WIN32_WINNT

namespace http
{
  class CFileSystem : public AMiddleware
  {
    protected:
    std::string mPath;
    
    public:
    CFileSystem(const std::string& root)
    {
      if(root[0] != '/' || root[1] != ':')
        mPath.append(os::getcwd()).append(DS).append(root);
      else
        mPath.append(root);
      // @todo Some settings...
      // buffersize
    }
    
//  std::string root("c:\\Games\\");
//  DIR* dir;
//  struct dirent* ent;
//  struct stat    st;
//  if((dir = ::opendir(root.c_str())) != NULL)
//  {
//    while((ent = ::readdir(dir)) != NULL)
//    {
//      const std::string name = ent->d_name;
//      const std::string path = root +"/"+ name;
//      
//      if(name[0] == '.')
//        continue;
//      
//      if(::stat(path.c_str(), &st) == -1)
//        continue;
//    
//      if((st.st_mode & S_IFDIR) != 0)
//        log::info << "[" << name << "]" << log::endl;  
//      else
//        log::info << name << log::endl;  
//    
//    
//    }
//    ::closedir(dir);
//  }
  
    public:
    void use(CServer& server)
    {
      server.on("request", [this](http::CRequest& req, http::CResponse& res) {
        log::info << mPath << " " << req.path() << log::endl;
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
