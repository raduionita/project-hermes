#ifndef __core_capp_hpp__
#define __core_capp_hpp__

namespace core
{
  class CApp
  {
    public:
    CApp();
    
    public:
    virtual CApp& run() = 0;
  };
}

#endif // __core_capp_hpp__
