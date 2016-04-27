#include <core/CException.hpp>

namespace core
{
  CException::CException(const std::string& message) : std::exception(), mMessage(message)
  {
    // do nothing
  }

  CException::CException(const std::string& message, const char* file, int line) : std::exception()
  {
    mMessage.append(file);
    mMessage.append(":");
    mMessage.append(std::to_string(line));
    mMessage.append(" ");
    mMessage.append(message);
  }

  CException::~CException()
  {
    // do nothing
  }

  const char* CException::what() const _GLIBCXX_USE_NOEXCEPT
  {
    return mMessage.c_str();
  }
  
  const char* CException::getMessage() const _GLIBCXX_USE_NOEXCEPT
  {
    return what();
  }
}
