#ifndef __core_cexception_hpp__
#define __core_cexception_hpp__

#include <exception>
#include <string>

#include <core.hpp>

namespace core
{
  class CException : public std::exception
  {
    protected:
    std::string mMessage;

    public:
    CException(const std::string& message) : std::exception(), mMessage(message)
    {
      // do nothing
    }
    
    CException(const std::string& message, const char* file, int line) : std::exception()
    {
      mMessage.append(file);
      mMessage.append(":");
      mMessage.append(std::to_string(line));
      mMessage.append(" ");
      mMessage.append(message);
    }
    
    virtual ~CException()
    {
      // do nothing
    }

    public:
    const char* what() const _GLIBCXX_USE_NOEXCEPT
    {
      return mMessage.c_str();
    }
    
    const char* getMessage() const _GLIBCXX_USE_NOEXCEPT
    {
      return what();
    }
  };
}

#endif //__core_cexception_hpp__
