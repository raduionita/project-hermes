#ifndef __log_clogger_hpp__
#define __log_clogger_hpp__

#include <core/CException.hpp>
#include <watch.hpp>

// @todo  Enable or disable logging
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

#define LOGERR log::error
#define LOGWRN log::warn
#define LOGNFO log::info

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

namespace log
{
  enum class ELevel : int
  {
    NONE  = 0x00,         //  0
    FATAL = 0x01 | NONE,  //  1
    ERROR = FATAL,        //  1
    WARN  = 0x02 | FATAL, //  3
    INFO  = 0x04 | WARN,  //  7
    DEBUG = 0x08 | INFO   // 15
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

  bool __needs_cr__ = false;

  class CLogger;

  class CLoggerStrategy
  {
    friend class CLogger;

    public:
    CLoggerStrategy()
    {
      //CLogger& log = CLogger::getInstance();
      //log << *this;
    }

    virtual ~CLoggerStrategy()
    {

    }

    public:
    virtual void log(const std::string& message) = 0;
  };

  void flush(CLoggerStrategy& strategy, const std::string& output)
  {
    strategy.log(output);
  }

  void endl(CLoggerStrategy& strategy, const std::string& output)
  {
    log::flush(strategy, (__needs_cr__ ? "\r" + output + "\n" : output + "\n"));
    __needs_cr__ = false;
  }

  void spinner(CLoggerStrategy& strategy)
  {
    static unsigned int i = 0;
    i = i % (1024 * 4);
    strategy.log(i < (1024 * 1) ? "\r-" : (i < (1024 * 2) ? "\r\\" : (i < (1024 * 3) ? "\r|" : "\r/")));
    ++i;
    __needs_cr__ = true;
  }

  void loading(CLoggerStrategy& strategy)
  {
    static unsigned int j = 0;
    j = j % (1024 * 4);
    strategy.log(j == (1024 * 0) ? "." : (j == (1024 * 1) ? "\r.." : (j == (1024 * 2) ? "\r\r..." : (j == (1024 * 3) ? "\r\r\r" : ""))));
    ++j;
    __needs_cr__ = true;
  }

  template <typename Rep, typename Period>
  std::basic_ostream<char>& operator <<(std::basic_ostream<char>& out, const std::chrono::duration<Rep, Period>& in)
  {
    return out<< in.count();
  }

  class CLogger
  {
    public:
    typedef void(*manipulator_t)(CLoggerStrategy&, const std::string&);
    typedef void(*special_t)(CLoggerStrategy&);

    protected:
    CLoggerStrategy*  mStrategy;
    std::stringstream mOutput;
    ELevel            mType;

    public:
    CLogger() : mStrategy(nullptr), mType(ELevel::INFO)
    {

    }

    CLogger(CLoggerStrategy* pStrategy)
    {
      mStrategy = pStrategy;
    }

    virtual ~CLogger()
    {
      *this << log::flush;
    }

    CLogger& operator <<(const ELevel& type)
    {
      mType = type;
      return *this;
    }

    CLogger& operator <<(manipulator_t manipulator)
    {
      size_t len = mOutput.tellp();

      if(len > 0 && LOGGING)
      {
        std::string level;
        switch(mType)
        {
          default:
          case ELevel::INFO:  level = " [INFO]  ";  break;
          case ELevel::DEBUG: level = " [DEBUG] "; break;
          case ELevel::WARN:  level = " [WARN]  ";  break;
          case ELevel::FATAL: level = " [FATAL] "; break;
        };

        manipulator(*mStrategy, watch::timestamp() + level + mOutput.str());
      }
      mOutput.str(std::string(""));
      return *this;
    }

    CLogger& operator <<(special_t special)
    {
      special(*mStrategy);
      return *this;
    }

    CLogger& operator <<(EColor color)
    {
      return *this;
    }

    template <typename T>
    CLogger& operator <<(const T& output)
    {
      mOutput << output;
      //ss << output;
      //mOutput.append(ss.str());
      return *this;
    }

    template <typename T>
    friend CLogger& operator <<(const ELevel&, const T&);

    public:
    CLogger& setStrategy(CLoggerStrategy* pStrategy)
    {
      mStrategy = pStrategy;
      return *this;
    }
  };

  class CFileLoggerStrategy : public CLoggerStrategy
  {
    friend class CLogger;

    protected:
    std::fstream mFStream;

    public:
    CFileLoggerStrategy(const std::string& file) : CLoggerStrategy()
    {
      //mFStream.open(file.getFilePath().c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
      mFStream.open(file.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
      if(!mFStream.is_open())
        throw core::CException("Failed to open/create log file");
        //throw EXCEPTION << "Failed to open/create log file: " << file.getFilePath();
    }

    ~CFileLoggerStrategy()
    {
      mFStream.close();
    }

    public:
    void log(const std::string& output)
    {
      mFStream.write(output.c_str(), output.size());
    }

    void log(const char output)
    {
      mFStream.write(&output, 1);
    }
  };

  class CCoutLoggerStrategy : public CLoggerStrategy
  {
    friend class CLogger;

    public:
    CCoutLoggerStrategy() : CLoggerStrategy()
    {
      std::cout.sync_with_stdio(true);
    }

    public:
    void log(const char output)
    {
      std::cout << output;
    }

    void log(const std::string& output)
    {
      std::cout << output;
    }
  };

  CCoutLoggerStrategy cout;
  CLogger             log(&cout);
  const char          tab('\t');
  const char          cr('\r');
  const char          nl('\n');

  ELevel info  = ELevel::INFO;
  ELevel debug = ELevel::DEBUG;
  ELevel warn  = ELevel::WARN;
  ELevel error = ELevel::FATAL;

  template <typename T>
  CLogger& operator << (ELevel& type, T const& output)
  {
    log << type;
    log << output;
    return log;
  }

  CLoggerStrategy& operator >> (CLoggerStrategy& oStrategy, CLogger& oLog)
  {
    oLog.setStrategy(&oStrategy);
    return oStrategy;
  }
}

#endif // __log_clogger_hpp__
