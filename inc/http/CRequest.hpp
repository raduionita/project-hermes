#ifndef __http_crequest_hpp__
#define __http_crequest_hpp__

#include <net.hpp>
#include <http.hpp>
#include <watch.hpp>

#include <log/CLogger.hpp>

#include <string>
#include <cstdio>
#include <map>

namespace http
{
  class CRequest
  {  
    friend class CServer;
  
    typedef std::string param_t;
    typedef std::string value_t;
    
    protected:
    socket_t                  mSocket;
    watch::milliseconds       mTime;
    
    EVerb                     mVerb;
    std::string               mPath;
    hashmap<param_t, value_t> mParams;
    hashmap<param_t, value_t> mQuery;
    hashmap<int,   value_t>   mHead;
    std::string               mBody;
    
    public:
    CRequest(socket_t& socket, const CMessage& msg)
    : mSocket(socket), mTime(watch::millitime()), mVerb(EVerb::GET), mPath("/"), mBody(msg.mMessage)
    {
      log::info << "http::CRequest::CRequest(sock, msg)" << log::endl;
      
      std::istringstream stream(msg);
      size_t i = 0;
      for(std::string line; std::getline(stream, line, '\n'); ++i)
      {
        line.pop_back(); // remove \r from the end
        if(line.empty()) // the body starts here
        {
          ++i;
          break;
        }
        else if(i == 0)  // VERB /path HTTP/1.1
        {
          char v [7];
          char p [512]; // 512 url limit
          char t [8];
          sscanf(line.c_str(), "%s %s %s", v, p, t);
          
          // @todo maybe a v[?] == 'P' would be a better check - optimal/faster
          if(strcmp(v, "POST") == 0) // request
            mVerb = EVerb::POST;
          else if(strcmp(v, "PUT") == 0)
            mVerb = EVerb::PUT;
          else if(strcmp(v, "OPTIONS") == 0)
            mVerb = EVerb::OPTIONS;
          else if(strcmp(v, "DELETE") == 0)
            mVerb = EVerb::DELETE;
          else if(strcmp(v, "HEAD") == 0)
            mVerb = EVerb::HEAD;
          else if(strcmp(v, "TRACE") == 0)
            mVerb = EVerb::TRACE;
          else if(strcmp(v, "CONNECT") == 0)
            mVerb = EVerb::CONNECT;
          else if(strcmp(v, "PATCH") == 0)
            mVerb = EVerb::PATCH;
            
          mPath = p;
        }
        else             // headers
        {
          size_t pos = line.find_first_of(':');
          param_t key(line.substr(0, pos));
          core::tolower(key);
          value_t val(line.substr(pos+2)); // remove first emptry char
          
          int head = 0;
          
          if(key == "accept")
            head = EHead::ACCEPT;
          else if(key == "charset")
            head = EHead::CHARSET;
          else if(key == "host")
            head = EHead::HOST;
          else if(key == "accept-encoding")
            head = EHead::ENCODING;
          else if(key == "accept-language")
            head = EHead::LANGUAGE;
          else if(key == "user-agent")
            head = EHead::USERAGENT;
          
          
          
          
          
          
          
          
          
          
          
          
          
          mHead[head] = val;
        }
      }
      
      // @todo Handle body...
    }
    
    CRequest(CRequest&& that) 
    : mSocket(that.mSocket)
    {
      log::info << "http::CRequest::CRequest(that&&)" << log::endl;
      mTime   = std::move(that.mTime);
      mVerb   = std::move(that.mVerb);
      mPath   = std::move(that.mPath);
      mParams = std::move(that.mParams);
      mHead   = std::move(that.mHead);
      mBody   = std::move(that.mBody);
    }
    
    virtual ~CRequest()
    {
      log::info << "http::CRequest::~CRequest()" << log::endl;
    }
    
    public:
    socket_t socket() const
    {
      return mSocket;
    }
    
    EVerb verb() const
    {
      return mVerb;
    }
    
    std::string path() const
    {
      return mPath;
    }
    
    value_t param(const param_t& key) const
    {
      auto it = mParams.find(key);
      if(it == mParams.end())
        return "undefined";
      return it->second;
    }
  
    value_t head(EHead key) const
    {
      auto it = mHead.find(static_cast<int>(key));
      if(it == mHead.end())
        return "undefined";
      return it->second;
    }
    
    hashmap<param_t, value_t> query() const
    {
      return mQuery;
    }
    
    std::string body() const
    {
      return mBody;
    }
  
    watch::milliseconds time() const
    {
      return mTime;
    }
  };
}

#endif // __http_crequest_hpp__
