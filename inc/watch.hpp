#ifndef __watch_hpp__
#define __watch_hpp__

#include <core.hpp>

#include <chrono>
#include <ctime>
#include <string>

namespace watch
{
  using hours        = std::chrono::hours;
  using minutes      = std::chrono::minutes;
  using seconds      = std::chrono::seconds;
  using microseconds = std::chrono::microseconds;
  using milliseconds = std::chrono::milliseconds;
  using nanoseconds  = std::chrono::nanoseconds;
  
  using timeinfo_t = struct tm;

  inline std::chrono::seconds time()
  {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  }
  
  inline std::chrono::microseconds microtime()
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
  }
  
  inline std::chrono::milliseconds millitime()
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
  }
  
  inline std::chrono::nanoseconds nanotime()
  {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
  }
  
  inline std::string timestamp()
  {
    std::time_t _time;
    timeinfo_t* _timeinfo;
    char        _timestr[22];
    
    std::time(&_time);
    _timeinfo = std::localtime(&_time);
    
    sprintf(_timestr, "[%04i-%02i-%02i %02i:%02i:%02i]", _timeinfo->tm_year + 1900, _timeinfo->tm_mon, _timeinfo->tm_mday, _timeinfo->tm_hour, _timeinfo->tm_min, _timeinfo->tm_sec);
    
    return std::string(_timestr);
  }
}

#endif // __watch_hpp__
