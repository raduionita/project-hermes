#ifndef __core_cevent_hpp__
#define __core_cevent_hpp__

#include <core.hpp>
#include <log/CLogger.hpp>

#include <iostream>

namespace core
{
  class CEvent
  {
    public:
    label_t mLabel;

    protected:
    CEvent(label_t label) : mLabel(label)
    {
      log::debug << "core::CEvent::CEvent(" << label << ")" << log::endl;
    }

    public:
    CEvent(const CEvent& that)
    {
      mLabel = that.mLabel;
    }

    CEvent(CEvent&& that)
    {
      mLabel = std::move(that.mLabel);
    }

    public:
    virtual ~CEvent()
    {
      log::debug << "core::CEvent::~CEvent()" << log::endl;
    }
  };
}

#endif // __core_cevent_hpp__
