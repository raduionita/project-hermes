#ifndef __net_crequest_hpp__
#define __net_crequest_hpp__

#include <net/CMessage.hpp>
#include <net/CSocket.hpp>

namespace net
{
  class CTcpServer;
  class CHttpServer;

  class CRequest : public CTcpMessage
  {
    friend class CTcpServer;
    friend class CHttpServer;
    
    public:
    CRequest(CTcpMessage& msg) : CTcpMessage(msg)
    {
      // parse msg
    }
  };
  
  class CResponse : public CTcpMessage
  {
    friend class CTcpServer;
    friend class CHttpServer;
    
    public:
    CResponse(socket_t socket) : CTcpMessage(socket)
    {
      
    }
    
    CResponse& write(const std::string& data)
    {
      
      return *this;
    }
    
    CResponse& end(const std::string& data = "")
    {
      
      return *this;
    }
    
    CResponse& set(const std::string& head, const std::string& value)
    {
      
      return *this;
    }
    
    CResponse& status(int status, const std::string& message)
    {
      
      return *this;
    }
  };
}

#endif // __net_crequest_hpp__
