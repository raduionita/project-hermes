#include <core/CError.hpp>

namespace core
{
  std::ostream& operator << (std::ostream& out, const CError& err)
  {
    out << "Error:" << err.mMessage;
    return out;
  }
}
