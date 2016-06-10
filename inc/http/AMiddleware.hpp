#ifndef __http_amiddleware_hpp__
#define __http_amiddleware_hpp__

namespace http
{
  class CApp;

  class AMiddleware
  {
    public:
    virtual ~AMiddleware()
    {
      
    }
    
    public:
    virtual void use(http::CApp& app) = 0;
  };
}

#endif // __http_amiddleware_hpp__
