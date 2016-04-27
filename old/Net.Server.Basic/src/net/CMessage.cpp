#include <net/CMessage.hpp>

namespace net
{
  CMessage::CMessage() : mStatus(RESULT_CLOSED)
  {
    
  }
  
  CMessage::CMessage(const std::string& msg) :  mMessage(msg), mStatus(RESULT_CLOSED)
  {
    
  }
  
  CMessage::~CMessage()
  {
    
  }
  
  CMessage& CMessage::append(const char* chunk, size_t length)
  {
    mMessage.append(chunk, length);
    return *this;
  }
  
  CMessage& CMessage::append(const char ch)
  {
    mMessage += ch;
    return *this;
  }
  
  CMessage& CMessage::status(status_t status)
  {
    mStatus = status;
    return *this;
  }
  
  status_t CMessage::status() const
  {
    return mStatus;
  }

  size_t CMessage::size() const
  {
    return mMessage.size();
  }
  
  CMessage::operator std::string()
  {
    return mMessage;
  }
  
  CMessage::operator const std::string() const
  {
    return mMessage;
  }
  
  CMessage::operator const char*() const
  {
    return mMessage.c_str();
  }
  
  std::ostream& operator << (std::ostream& out, const CMessage& msg)
  {
    out << msg.mMessage;
    return out;
  }
}
