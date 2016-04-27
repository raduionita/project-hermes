#ifndef __core_cevent_hpp__
#define __core_cevent_hpp__

namespace core
{
  class CEvent 
  {
    public:
    label_t mLabel;
    
    protected:
    CEvent(label_t label) : mLabel(label)
    {
    
    }
    
    public:
    virtual ~CEvent()
    {
      // do nothing
    }
  };
}

#endif // __core_cevent_hpp__
