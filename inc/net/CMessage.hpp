#ifndef __net_cmessage_hpp__
#define __net_cmessage_hpp__

#include <net.hpp>

#include <string>

namespace net
{
  class CMessage
  {
    public:
    std::string mMessage;
    
    public:
    CMessage()
    {
      log::info << "net::CMessage::CMessage()" << log::endl;
    }
    
    CMessage(const CMessage& that)
    {
      log::info << "net::CMessage::CMessage(that&)" << log::endl;
      mMessage = that.mMessage;
    }
    
    CMessage(CMessage&& that)
    {
      log::info << "net::CMessage::CMessage(that&&)" << log::endl;
      mMessage = std::move(that.mMessage);
    }
    
    CMessage(const std::string& message) : mMessage(message)
    {
      log::info << "net::CMessage::CMessage(message)" << log::endl;
    }
    
    virtual ~CMessage()
    {
      log::info << "net::CMessage::~CMessage()" << log::endl;
    }
    
    CMessage& operator =(CMessage& that)
    {
      log::info << "net::CMessage::operator =(that&)" << log::endl;
      if(this != &that)
      {
        mMessage = that.mMessage;
      }
      return *this;
    }
    
    CMessage& operator =(CMessage&& that)
    {
      log::info << "net::CMessage::operator =(that&&)" << log::endl;
      if(this != &that)
      {
        mMessage = std::move(that.mMessage);
      }
      return *this;
    }
    
    operator std::string ()
    {
      return mMessage;
    }
    
    operator const std::string () const
    {
      return mMessage;
    }
    
    operator const char* () const
    {
      return mMessage.c_str();
    }
    
    public:
    CMessage& append(const char* chunk, size_t length);
    CMessage& append(const char ch);
    CMessage& status(status_t status);
    status_t  status() const;
    
    size_t    size() const
    {
      return mMessage.size();
    }
  
    friend std::ostream& operator << (std::ostream&, const CMessage&);
  };
  
  std::ostream& operator <<(std::ostream& out, const CMessage& msg)
  {
    out << msg.mMessage;
    return out;
  }
}

#endif // __net_cmessage_hpp__
