#ifndef __net_cconnection_hpp__
#define __net_cconnection_hpp__

#include <net.hpp>
#include <core/CEventListener.hpp>
#include <core/CBuffer.hpp>

#include <iostream>

namespace net
{
  class CTcpServer;

  class CConnection : public core::CEventListener
  {
    friend class CTcpServer;
  
    public:
    enum EState
    {
      #undef ERROR
      ERROR  =    -1,
      NONE   =  0x00,
      IDLE   =  0x01,
      FLUSH  =  0x02,
      DONE   =  0x04 + 0x02,
      CLOSED =  0x08
    };
    
    public:
    CSocket     mSocket;
    std::string mBuffer;
    EState      mState;
    
    public:
    CConnection(socket_t socket) : mSocket(socket), mState(EState::IDLE)
    {
      std::cout << "> New connection." << std::endl;
    }
    
    CConnection(const CConnection& that) : mSocket(that.mSocket), mState(that.mState)
    {
      // ...
    }
    
    virtual ~CConnection()
    {
      
    }
  
    public:
    void on(const label_t& label, const std::function<void(void)>& callback)
    {
      mActions[label] = [callback](CEvent& e) {
        callback();
      };
    }
    
    void on(const label_t& label, const std::function<void(CMessage&)>& callback)
    {
      mActions[label] = [callback](CEvent& e) {
        CMessageEvent& me = dynamic_cast<CMessageEvent&>(e);
        callback(me.mMessage);
      };
    }
  
    void write(const std::string& data) 
    { 
      mBuffer.append(data);
      if(mBuffer.size() > 4096) 
        flush();
    }
    
    void flush()
    {
      mState = EState::FLUSH;
    }
    
    void end(const std::string& data) 
    {
      mBuffer.append(data);
      mState = EState::DONE;
    }
    
    void end(const CMessage& msg) 
    {
      mBuffer.append(msg.mMessage);
      mState = EState::DONE;
    }
    
    void close()
    {
      mSocket.close();
      mState = EState::CLOSED;
    }
  };
}

#endif // __net_cconnection_hpp__
