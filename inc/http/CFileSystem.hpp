#ifndef __http_cfilesystem_hpp__
#define __http_cfilesystem_hpp__

#include <http.hpp>
#include <http/AMiddleware.hpp>
#include <http/CApp.hpp>
#include <os.hpp>
#include <log/CLogger.hpp>

#ifdef _WIN32_WINNT
  #define DS "\\"
#else // LINUX
  #define DS "/"
#endif // _WIN32_WINNT

#include <string>
#include <vector>

namespace http
{
  using namespace core;

  class CFileSystem : public AMiddleware
  {
    protected:
    std::vector<std::string> mPaths;
    std::string              mRoot;

    public:
    CFileSystem(const std::string& root)
    {
      log::debug << "http::CFileSystem::CFileSystem()" << log::endl;

      if(root[0] != '/' || root[1] != ':')
        mRoot.append(os::getcwd()).append(DS).append(root);
      else
        mRoot.append(root);

      // @todo Some settings...
      // buffersize
    }

/*
  std::string root("c:\\Games\\");
  DIR* dir;
  struct dirent* ent;
  struct stat    st;
  if((dir = ::opendir(root.c_str())) != NULL)
  {
    while((ent = ::readdir(dir)) != NULL)
    {
      const std::string name = ent->d_name;
      const std::string path = root +"/"+ name;

      if(name[0] == '.')
        continue;

      if(::stat(path.c_str(), &st) == -1)
        continue;

      if((st.st_mode & S_IFDIR) != 0)
        log::info << "> [" << name << "]" << log::endl;
      else
        log::info << name << log::endl;


    }
    ::closedir(dir);
  }
*/

    public:
    void use(http::CApp& app)
    {
      log::debug << "http::CFileSystem::use(http::CApp)" << log::endl;

      static bool bound = false;
      if(bound) // only one bind
      {
        log::warn << "> Only one filesystem middleware...for now!" << log::endl;
        return;
      }

      app.mServer.on("request", [this](http::CRequest& req, http::CResponse& res) {
        log::info << "> CFileSystem ... CServer::on(request, callback)" << log::endl;
        if(req.verb() == EVerb::GET)
        {
          std::string path;
          path.append(mRoot).append(req.path());

          // @todo Find a better way to check if the request is a file, use allowed file types

          // @todo use <sys/stat.h>::stat() to check if file exits

          std::ifstream ifs(path.c_str(), std::ios::binary | std::ios::in);
          if(ifs)
          {
            // @todo Setup header
            std::size_t dpos = path.find_last_of('.');
            std::string  ext = path.substr(dpos+1);

            // @todo Send the correct header, try to use file extension/type
            // res.type(http::ICON);
            res.head(EHead::TYPE, req.head(EHead::ACCEPT));

            std::string output;
            ifs.seekg(0, std::ios::end);
            output.reserve(ifs.tellg());
            ifs.seekg(0, std::ios::beg);
            output.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

            res.status(http::OK);
            res.send(output);

            log::info << "> Sending... File: " << req.path() << " Extension: " << ext << log::endl;

            // @todo Send files
            // res.send(fio.read("relative/path/to/file", file::UTF8));

            // @todo Save the request into a cache, and use that...

            // @todo Set a large expiration date, check if the file was changed

            res.end();
          }
        }
      });

      bound = true;
/*
         trim / and dots(..)
         try to open file
         if fail...404

         read chunks of file in a loop
         while(still data to read)
           if(res.size() == full)
             continue;
           res.send(chunk)
         res.end();

         keep max 256 files open

         cache(w/ timeout) small files < 1M!?
*/
    }
  };
}

#endif // __http_cfilesystem_hpp__
