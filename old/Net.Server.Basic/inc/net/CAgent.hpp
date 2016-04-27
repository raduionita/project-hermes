#ifndef __net_cagent_hpp__
#define __net_cagent_hpp__

#include <net.hpp>

namespace net
{
  class CAgent
  {
    public:
    enum EType
    {
      CLIENT = 0,
      SERVER = 1
    };
    
    public:
    CAgent()
    {
    
    }
    
    virtual ~CAgent()
    {
      
    }
  
    public:
    virtual EType getType() const = 0;
  };
}

#endif // __net_cagent_hpp__
