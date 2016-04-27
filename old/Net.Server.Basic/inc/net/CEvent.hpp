#ifndef __net_cevent_hpp__
#define __net_cevent_hpp__

namespace net
{
  class CEvent
  {
    typedef const std::string label_t;
    
    public:
    virtual ~CEvent() { } // w/o virtual is not polymorphic
  };
}

#endif // __net_cevent_hpp__
