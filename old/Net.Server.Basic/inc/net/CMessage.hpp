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
    status_t    mStatus;
    
    public:
    CMessage();
    CMessage(const std::string&);
    virtual ~CMessage();
    
    operator std::string ();
    operator const std::string () const;
    
    operator const char* () const;
    
    public:
    CMessage& append(const char* chunk, size_t length);
    CMessage& append(const char ch);
    CMessage& status(status_t status);
    status_t  status() const;
    size_t    size() const;
  
    friend std::ostream& operator << (std::ostream&, const CMessage&);
  };
  
  class CTcpMessage : public CMessage
  {
    protected:
    socket_t mSocket;
  
    public:
    CTcpMessage() : CMessage(), mSocket(0) 
    { 
    
    }
    
    public:
    CTcpMessage(socket_t socket) : CMessage(), mSocket(socket) 
    { 
    
    }
    
    CTcpMessage(socket_t socket, const CMessage& that) : CMessage(), mSocket(socket) 
    { 
      mMessage = that.mMessage;
      mStatus  = that.mStatus;
    }
    
    CTcpMessage(const CTcpMessage& that)
    {
      mMessage = that.mMessage;
      mStatus  = that.mStatus;
      mSocket  = that.mSocket;
    }
  };
}

#endif // __net_cmessage_hpp__
