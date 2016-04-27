#include <net\CClient.hpp>

namespace net 
{
  CClient::CClient()
  {
    //ctor
  }

  CClient::~CClient()
  {
    //dtor
  }

  CClient::CClient(const CClient& that)
  {
    //copy ctor
  }

  CClient& CClient::operator = (const CClient& that)
  {
    if(this == &that) return *this;
    
    
    
    return *this;
  }
}
