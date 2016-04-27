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
    CException(const std::string&);
    CException(const std::string&, const char*, int);
    virtual ~CException();

    public:
    const char* what() const _GLIBCXX_USE_NOEXCEPT;
    const char* getMessage() const _GLIBCXX_USE_NOEXCEPT;
  };
}

#endif //__core_cexception_hpp__
