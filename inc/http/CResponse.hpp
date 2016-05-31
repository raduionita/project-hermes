#ifndef __http_cresponse_hpp__
#define __http_cresponse_hpp__

#include <watch.hpp>
#include <http.hpp>

#include <core/CSynchronizable.hpp>
#include <log/CLogger.hpp>

#include <string>

namespace http
{
  class CResponse : public core::CSynchronizable
  {
    friend class CServer;
  
    protected:
    socket_t                    mSocket;
    watch::milliseconds         mTime;
    EState                      mState;
    
    uint                        mStatus;
    hashmap<int, std::string>   mHead;    // should be EHead, string
    std::string                 mBody;    // this should be a CBuffer<char>
    
    public:
    CResponse(socket_t sock) 
    : mSocket(sock), mTime(watch::millitime())
    {
      log::info << "http::CResponse::CResponse(sock)" << log::endl;
      mStatus            = http::OK;
      mState             = EState::IDLE;
      mHead[EHead::TYPE] = "text/plain";
    }
    
    CResponse(CResponse&& that)
    : mSocket(that.mSocket)
    {
      log::info << "http::CResponse::CResponse(that&&)" << log::endl;
      mTime   = std::move(that.mTime);
      mHead   = std::move(that.mHead);
      mBody   = std::move(that.mBody);
      mStatus = std::move(that.mStatus);
      mState  = std::move(that.mState);
    }
    
    ~CResponse()
    {
      log::info << "http::CResponse::~CResponse()" << log::endl;
    }
    
    public:
    socket_t socket() const
    {
      return mSocket;
    }
    
    bool is(EState state) const
    {
      return mState == state;
    }
    
    EState state() const
    {
      return mState;
    }
    
    CResponse& status(EStatus val)
    {
      mStatus = val;
      head(EHead::STATUS, std::to_string(static_cast<int>(val)));
      return *this;
    }
    
    CResponse& head(EHead key, const std::string& value)
    {
      if(mState != EState::IDLE) 
        log::warn << "> Setting headers after you started flusing is useless!" << log::endl;
      else
        mHead[key] = value;
      // @todo Some processing may be needed
      return *this;
    }
    
    std::string head(EHead key) const
    {
      auto it = mHead.find(key);
      if(it == mHead.end())
        return "undefined";
      return it->second;
    }
    
    CResponse& type(EType val)
    {
      head(EHead::TYPE, http::getType(val));
      return *this;
    }
    
    std::string type() const
    {
      return head(EHead::TYPE);
    }
    
    CResponse& charset(const std::string value)
    {
      // @todo Needs implementation
      return *this;
    }
    
    CResponse& send(const std::string& data)
    {
      if(mState == EState::DONE)
      {
        log::warn << "> Writing after the response was marked as ended is useless!" << log::endl;
      }
      else
      {
        mBody.append(data);
        mState = mState == EState::IDLE ? EState::STARTED : mState; // change only if lower
      }
      return *this;
    }
    
    //CResponse& send(const core::CFile& file)
    //{
    //  // @todo send ... file
    //}
    
    CResponse& end(const std::string& data)
    {
      send(data);
      return end();
    }
    
    CResponse& end()
    {
      mState = EState::DONE;
      return *this;
    }
    
    protected:
    watch::milliseconds time() const
    {
      return mTime;
    }
    
    std::string read()
    {
      std::string output("HTTP/1.1 ");
      
      switch(mStatus)
      {
        default:
        case EStatus::OK         : output.append("200 OK");           break;
        case EStatus::NOCONTENT  : output.append("204 No Content");   break;
        case EStatus::MOVED      : output.append("301 Moved");        break;
        case EStatus::NOTMODIFIED: output.append("304 Not Modified"); break;
        case EStatus::BADREQUEST : output.append("400 Bad Request");  break;
        case EStatus::FORBIDDEN  : output.append("403 Forbidden");    break;
        case EStatus::NOTFOUND   : output.append("404 Not Found");    break;
        case EStatus::SERVERERROR: output.append("500 Server Error"); break;
        case EStatus::UNAVAILABLE: output.append("503 Unavailable");  break;
        case EStatus::LOOP       : output.append("508 Loop");         break;
      }
      output.append("\r\n");
      for(auto it = mHead.begin(); it != mHead.end(); ++it)
      {
        EHead        param  = static_cast<EHead>(it->first);
        std::string& value  = it->second;
        switch(param)
        {
          default               : break;
          case EHead::ACCEPT    : break;
          case EHead::CHARSET   : break;
          case EHead::ENCODING  : break;
          case EHead::CACHE     : break;
          case EHead::CONNECTION: output.append("Connection: "); break;
          case EHead::COOKIE    : break; // += request
          case EHead::LENGTH    : break; // output += "Content-Length: ";
          case EHead::TYPE      : output.append("Content-Type: "); break;
          case EHead::DATE      : break;
          case EHead::EXPECT    : break;
          case EHead::FORWARDED : break;
          case EHead::FROM      : break;
          case EHead::HOST      : break;
          case EHead::ORIGIN    : break;
          case EHead::PRAGMA    : break;
          case EHead::PROXY     : break;
          case EHead::RANGE     : break;
          case EHead::REFERER   : break;
          case EHead::USERAGENT : break;
          case EHead::UPGRADE   : break;
          ;
          case EHead::ETAG      : break;
          case EHead::AGE       : break;
          case EHead::EXPIRES   : break;
          case EHead::MODIFIED  : break;
          case EHead::SERVER    : break;
          case EHead::STATUS    : output += "Status: "; break;
          ;
          case EHead::CSRF      : break;
        }
        output.append(value).append("\r\n");
      }
      
      output.append("Content-Length: ").append(std::to_string(mBody.size())).append("\r\n\r\n");
      
      output.append(mBody);
      
      return output;
    }
    
    size_t size() const
    {
      return mBody.size();
    }
  };
}

#endif // __http_cresponse_hpp__
