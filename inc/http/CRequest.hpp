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
    typedef std::string body_t;

    protected:
    socket_t                  mSocket;
    watch::milliseconds       mTime;

    EVerb                     mVerb;
    std::string               mPath;
    EHttp                     mHttp;
    hashmap<param_t, value_t> mParams;
    hashmap<param_t, value_t> mQuery;
    hashmap<int,     value_t> mHead; // replace with CHeader
    body_t                    mBody;

    public:
    CRequest(socket_t& socket, const CMessage& msg)
    : mSocket(socket), mTime(watch::millitime()), mVerb(EVerb::GET), mPath("/"), mHttp(EHttp::HTTP10), mBody(msg.mMessage)
    {
      log::debug << "http::CRequest::CRequest(sock, msg)" << log::endl;

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
          int count = sscanf(line.c_str(), "%s %s %s", v, p, t);

          log::info << "> " << v << " " << p << " " << t << log::endl;

          if (count > 0) // verb found
          {
            // verb
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
            // @todo Add remaining VERBS/METHODS
          }

          if (count > 1) // path found
          {
            // @todo Move query string into mQuery
            mPath = p;
          }

          if (count > 2) // http found
          {
            // type default: HTTP/1.0 (close if not mention otherwise)
            // @todo Need a better way to check this string
            if(strcmp(t, "1.1") == 0)
              mHttp = EHttp::HTTP11;          // HTTP/1.1 (most popular 1st)
            else if(strcmp(t, "0.9") == 0)
              mHttp = EHttp::HTTP09;          // HTTP/0.9
            else if(strcmp(t, "2.0") == 0)
              mHttp = EHttp::HTTP20;          // HTTP/2.0
            else // if(strcmp(t, "1.0") == 0)
              mHttp = EHttp::HTTP10;          // HTTP/1.0
          }
        }
        else             // headers
        {
          EHead head = EHead::__HEAD__;

          size_t pos = line.find_first_of(':');
          param_t key(line.substr(0, pos));
          core::tolower(key);
          // find key
          if(key == "accept")
            head = EHead::ACCEPT;
          else if(key == "charset")
            head = EHead::CHARSET;
          else if(key == "host")
            head = EHead::HOST;
          else if(key == "accept-encoding")
            head = EHead::ENCODING;
          else if(key == "connection")
            head = EHead::CONNECTION;
          else if(key == "accept-language")
            head = EHead::LANGUAGE;
          else if(key == "user-agent")
            head = EHead::USERAGENT;
          else if(key == "content-type")
            head = EHead::TYPE;
          // @todo Add missing headers

          value_t val(line.substr(pos+2)); // remove first emptry char
          core::tolower(key);
          // set value
          mHead[head] = val;
        }
      }



      // @todo Handle body...



    }

    CRequest(CRequest&& that)
    : mSocket(that.mSocket)
    {
      log::debug << "http::CRequest::CRequest(that&&)" << log::endl;
      mTime   = std::move(that.mTime);
      mVerb   = std::move(that.mVerb);
      mPath   = std::move(that.mPath);
      mParams = std::move(that.mParams);
      mHead   = std::move(that.mHead);
      mBody   = std::move(that.mBody);
    }

    virtual ~CRequest()
    {
      log::debug << "http::CRequest::~CRequest()" << log::endl;
    }

    public:
    socket_t socket() const
    {
      return mSocket;
    }

    EHttp http()
    {
      return mHttp;
    }

    EVerb verb() const
    {
      return mVerb;
    }

    inline std::string path() const
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

    CRequest& param(const param_t& k, const value_t& v)
    {
      mParams[k] = v;
      return *this;
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

    body_t body() const
    {
      return mBody;
    }

    value_t type() const
    {
      return head(EHead::TYPE);
    }

    watch::milliseconds time() const
    {
      return mTime;
    }
  };
}

#endif // __http_crequest_hpp__
