#include <net/CError.hpp>

namespace net
{
  std::ostream& operator << (std::ostream& out, const CError& err)
  {
    out << "Error " << err.mCode << ": " << err.mMessage;
    return out;
  }
}
