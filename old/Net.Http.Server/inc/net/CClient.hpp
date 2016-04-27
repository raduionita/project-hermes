#ifndef __net_cclient_hpp__
#define __net_cclient_hpp__

#include <core.hpp>

namespace net
{
  class CClient
  {
    public:
    CClient();
    
    virtual ~CClient();
    
    CClient(const CClient&);
    
    CClient& operator = (const CClient&);
  };
}

#endif // __net_cclient_hpp__
