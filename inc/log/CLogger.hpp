#ifndef __log_clogger_hpp__
#define __log_clogger_hpp__

#include <core/CException.hpp>

// enable or disable logging
#ifndef LOGGING
  #define LOGGING 1
#endif // LOGGING

// @todo Loggin level - what to log
#ifndef LOGLEVEL
  #define LOGLEVEL log::ELevel::INFO
#endif // LOGLEVEL

// @todo Thread safe logger
#ifndef LOGTHREAD
  #define LOGTHREAD 0
#endif // LOGTHREAD

// @todo Log time ifndef LOGTIME = null or LOGTIME = 1
#ifndef LOGTIME
  #define LOGTIME 1
#endif // LOGTIME

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

namespace log
{ 
  enum class ELevel : int
  {
    NONE  = 0x00,
    FATAL = 0x01 | NONE,
    ERROR = FATAL,
    WARN  = 0x02 | FATAL,
    DEBUG = 0x04 | WARN,
    INFO  = DEBUG,
  };
  
  enum class EOption : int
  {
    TAG  = 0x1,
    TIME = 0x2,
  };
  
  enum class EColor : int
  {
    BLACK    = 0,
    DARKBLUE = 1, 
    DARKGREEN, 
    DARKTEAL, 
    DARKRED, 
    DARKPINK, 
    DARKYELLOW, 
    GRAY, 
    DARKGRAY, 
    BLUE, 
    GREEN, 
    TEAL, 
    RED, 
    PINK, 
    YELLOW, 
    WHITE
  };

  class CLogger;

  class CLoggerStrategy
  {
    friend class CLogger;
  
    public:
    CLoggerStrategy();
    virtual ~CLoggerStrategy();
  
    public:
    virtual void log(const std::string& message) = 0;
  };
  
  extern void flush(CLoggerStrategy& strategy, const std::string& output);
  
  extern void endl(CLoggerStrategy& strategy, const std::string& output);
  
  class CLogger
  {
    public:
    typedef void(*manipulator_t)(CLoggerStrategy&, const std::string&);
    typedef struct tm timeinfo_t;
  
    protected:
    CLoggerStrategy*  mStrategy;
    std::stringstream mOutput;
    ELevel             mType;
    time_t            mTime;
    
    public:
    CLogger();
    CLogger(CLoggerStrategy* pStrategy);
    virtual ~CLogger();
    
    CLogger& operator <<(const ELevel& type);
    CLogger& operator <<(manipulator_t manipulator);
    
    template <typename T>
    CLogger& operator <<(const T& output)
    {
      mOutput << output;
      //ss << output;
      //mOutput.append(ss.str());
      return *this;
    }
    
    CLogger& operator <<(EColor color);
    
    template <typename T>
    friend CLogger& operator <<(const ELevel&, const T&);
  
    public:
    CLogger& setStrategy(CLoggerStrategy* pStrategy);
  };

  class CFileLoggerStrategy : public CLoggerStrategy
  {
    friend class CLogger;
    
    protected:
    std::fstream mFStream;
    
    public:
    CFileLoggerStrategy(const std::string& file);
    ~CFileLoggerStrategy();
    
    public:
    void log(const std::string& output);
  };
  
  class CCoutLoggerStrategy : public CLoggerStrategy
  {
    friend class CLogger;
    
    public:
    CCoutLoggerStrategy();
    
    public:
    void log(const std::string& output);
  };

  extern CCoutLoggerStrategy cout;
  extern CLogger             log;
  extern const std::string   tab;

  extern ELevel info;
  extern ELevel debug;
  extern ELevel warn;
  extern ELevel error;

  template <typename T>
  CLogger& operator << (ELevel& type, T const& output)
  {
    log << type;
    log << output;
    return log;
  }

  extern CLoggerStrategy& operator >> (CLoggerStrategy& oStrategy, CLogger& oLog);
}

#endif // __log_clogger_hpp__
