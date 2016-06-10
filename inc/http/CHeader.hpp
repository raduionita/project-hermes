#ifndef __http_cheader_hpp__
#define __http_cheader_hpp__

#include <string>
#include <utility>
#include <vector>

namespace http
{
  class CHeader
  {
    typedef float                         quality_t;
    typedef std::string                   value_t;
    typedef std::pair<value_t, quality_t> part_t;
    
    protected:
    std::vector<part_t> mParts;
    
    
    public:
    CHeader()
    {
      // @todo Replace header strings with header object
      // 1st split at ,
      // 2nd split at ;
      // q=0.1 indicates the degree of preference for that media type, default =1
    }
    
    virtual ~CHeader()
    {
      
    }
  };
}

#endif // __http_cheader_hpp__
