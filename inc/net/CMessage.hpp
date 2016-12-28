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
    EProtocol   mProtocol;

    public:
    CMessage()
    {
      log::debug << "net::CMessage::CMessage()" << log::endl;
    }

    CMessage(const CMessage& that)
    {
      log::debug << "net::CMessage::CMessage(that&)" << log::endl;
      mMessage = that.mMessage;
    }

    CMessage(CMessage&& that)
    {
      log::debug << "net::CMessage::CMessage(that&&)" << log::endl;
      mMessage = std::move(that.mMessage);
    }

    CMessage(const std::string& message) : mMessage(message)
    {
      log::debug << "net::CMessage::CMessage(message)" << log::endl;
    }

    virtual ~CMessage()
    {
      log::debug << "net::CMessage::~CMessage()" << log::endl;
    }

    CMessage& operator =(CMessage& that)
    {
      log::debug << "net::CMessage::operator =(that&)" << log::endl;
      if(this != &that)
      {
        mMessage = that.mMessage;
      }
      return *this;
    }

    CMessage& operator =(CMessage&& that)
    {
      log::debug << "net::CMessage::operator =(that&&)" << log::endl;
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
    CMessage& append(const char* chunk)
    {
      mMessage.append(chunk);
      return *this;
    }

    CMessage& append(const char ch)
    {
      // @todo Finish this
      return *this;
    }

    CMessage& clear()
    {
      mMessage.erase();
      return *this;
    }

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
