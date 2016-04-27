#ifndef __core_cevent_hpp__
#define __core_cevent_hpp__

#include <vector>
#include <string>

namespace core
{
  class CEventType
  {
    typedef unsigned short value_t;
  
    protected:
    static std::vector<std::string> sTable;
    value_t                         mValue;
    
    public:
    CEventType(const std::string& label)
    {
      sTable.push_back(label);
      mValue = sTable.size() - 1;
    }
    
    CEventType(const CEventType& that)
    {
      mValue = that.mValue;
    }
    
    CEventType(CEventType&& that)
    {
      mValue = std::move(that.mValue);
    }
    
    CEventType& operator = (const CEventType& that)
    {
      if(this != &that)
        mValue = that.mValue;
      return *this;
    }
    
    CEventType& operator = (const CEventType&& that)
    {
      if(this != &that)
        mValue = std::move(that.mValue);
      return *this;
    }
    
    bool operator == (const CEventType& that)
    {
      return mValue == that.mValue;
    }
    
    explicit operator value_t ()
    {
      return mValue;
    }  
    
    explicit operator const value_t () const
    {
      return mValue;
    }
  
    operator const std::string () const
    {
      return sTable[mValue];
    }
  
    friend bool operator <  (const CEventType& lhs, const CEventType& rhs);
    
    friend bool operator == (const CEventType& lhs, const CEventType& rhs);
    
    friend bool operator != (const CEventType& lhs, const CEventType& rhs);
    
    public:
    const std::string& getLabel() const
    {
      return sTable[mValue];
    }
  };
  std::vector<std::string> CEventType::sTable;
  
  bool operator <  (const CEventType& lhs, const CEventType& rhs)
  {
    return lhs.mValue < rhs.mValue;
  }
  
  bool operator == (const CEventType& lhs, const CEventType& rhs)
  {
    return lhs.mValue == rhs.mValue;
  }
  
  bool operator != (const CEventType& lhs, const CEventType& rhs)
  {
    return lhs.mValue != rhs.mValue;
  }
  
  namespace event
  {
    CEventType ERROR("error");
    CEventType TICK("tick");
  };

  class CEvent
  {
    public:
    virtual CEventType& getType() const = 0;
  };
  
  class CErrorEvent : public CEvent
  {
    public:
    CEventType& getType() const
    {
      return event::ERROR;
    }
  };
  
  class CTickEvent : public CEvent
  {
    public:
    CEventType& getType() const
    {
      return event::TICK;
    }
  };
}

#endif // __core_cevent_hpp__
