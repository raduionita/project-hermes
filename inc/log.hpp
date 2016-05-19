#ifndef __log_hpp__
#define __log_hpp__

#include <core.hpp>
#include <log/CLogger.hpp>

namespace log
{
  extern CCoutLoggerStrategy cout;
  extern CLogger             log;
  extern const char          tab;
  extern const char          cr;
  extern const char          nl;

  extern ELevel info;
  extern ELevel debug;
  extern ELevel warn;
  extern ELevel error;
}

#endif // __log_hpp__
