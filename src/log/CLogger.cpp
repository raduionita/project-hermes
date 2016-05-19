#include <log/CLogger.hpp>

namespace log
{
  CLoggerStrategy::CLoggerStrategy()
  {
    //CLogger& log = CLogger::getInstance();
    //log << *this;
  }

  CLoggerStrategy::~CLoggerStrategy()
  {
    
  }

  bool __needs_cr__ = false;
  
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
  
  CLogger::CLogger() : mStrategy(nullptr), mType(ELevel::INFO)
  {
    
  }
  
  CLogger::CLogger(CLoggerStrategy* pStrategy)
  {
    mStrategy = pStrategy;
  }
  
  CLogger::~CLogger()
  {
    *this << log::flush;
  }
  
  CLogger& CLogger::operator <<(const ELevel& type)
  {
    mType = type;
    return *this;
  }

  CLogger& CLogger::operator <<(manipulator_t manipulator)
  {
    size_t len = mOutput.tellp();
    
    if(len > 0 && LOGGING)
    {
      std::string level;
      switch(mType)
      {
        default:
        case ELevel::INFO:  level = " [INFO] ";  break;
      //case ELevel::DEBUG: level = " [DEBUG] "; break;
        case ELevel::WARN:  level = " [WARN] ";  break;
        case ELevel::FATAL: level = " [FATAL] "; break;
      };

      manipulator(*mStrategy, watch::timestamp() + level + mOutput.str());
    }
    mOutput.str(std::string(""));
    return *this;
  }
  
  CLogger& CLogger::operator <<(special_t special)
  {
    special(*mStrategy);
    return *this;
  }
  
  CLogger& CLogger::operator <<(EColor color)
  {
    return *this;
  }

  CLogger& CLogger::setStrategy(CLoggerStrategy* pStrategy)
  {
    mStrategy = pStrategy;
    return *this;
  }

  CFileLoggerStrategy::CFileLoggerStrategy(const std::string& file) : CLoggerStrategy()
  {
    //mFStream.open(file.getFilePath().c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    mFStream.open(file.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    if(!mFStream.is_open())
      throw core::CException("Failed to open/create log file");
      //throw EXCEPTION << "Failed to open/create log file: " << file.getFilePath();
  }
  
  CFileLoggerStrategy::~CFileLoggerStrategy()
  {
    mFStream.close();
  }
  
  void CFileLoggerStrategy::log(const std::string& output)
  {
    mFStream.write(output.c_str(), output.size());
  }

  void CFileLoggerStrategy::log(const char output)
  {
    mFStream.write(&output, 1);
  }

  CCoutLoggerStrategy::CCoutLoggerStrategy() : CLoggerStrategy()
  {
    std::cout.sync_with_stdio(true);
  }
  
  void CCoutLoggerStrategy::log(const char output)
  {
    std::cout << output;
  }
  
  void CCoutLoggerStrategy::log(const std::string& output)
  {
    std::cout << output;
  }
  
  CCoutLoggerStrategy cout;
  CLogger             log(&cout);
  const char          tab('\t');
  const char          cr('\r');
  const char          nl('\n');

  ELevel info  = ELevel::INFO;
  ELevel debug = ELevel::DEBUG;
  ELevel warn  = ELevel::WARN;
  ELevel error = ELevel::FATAL;

  CLoggerStrategy& operator >> (CLoggerStrategy& oStrategy, CLogger& oLog)
  {
    oLog.setStrategy(&oStrategy);
    return oStrategy;
  }
}
