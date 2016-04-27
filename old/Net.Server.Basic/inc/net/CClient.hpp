#ifndef __net_cclient_hpp__
#define __net_cclient_hpp__

#include <net.hpp>
#include <net/CAgent.hpp>

namespace net
{
  class CClient : public CAgent
  {
    public:
    CClient() : CAgent()
    {
      
    }
    
    public:
    EType getType() const
    {
      return EType::CLIENT;
    }
  };
}

#endif // __net_cclient_hpp__
