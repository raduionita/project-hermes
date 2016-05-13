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

  void flush(CLoggerStrategy& strategy, const std::string& output)
  {
    strategy.log(output);
  }
  
  void endl(CLoggerStrategy& strategy, const std::string& output)
  {
    log::flush(strategy, output + "\n");
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
        case ELevel::INFO:  level = "[INFO] ";  break;
      //case ELevel::DEBUG: level = "[DEBUG] "; break;
        case ELevel::WARN:  level = "[WARN] ";  break;
        case ELevel::FATAL: level = "[FATAL] "; break;
      };

      static timeinfo_t* timeinfo;
      std::time(&mTime);
      timeinfo = std::localtime(&mTime);
      
      char timestr[23];
      std::strftime(timestr, 23, "[%Y-%m-%d %H:%M:%S] ", timeinfo);
      timestr[22] = '\0';
      manipulator(*mStrategy, std::string(timestr) + level + mOutput.str());
    }
    mOutput.str(std::string(""));
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

  CCoutLoggerStrategy::CCoutLoggerStrategy() : CLoggerStrategy()
  {
    std::cout.sync_with_stdio(true);
  }
  
  void CCoutLoggerStrategy::log(const std::string& output)
  {
    std::cout << output;
  }
  
  CCoutLoggerStrategy cout;
  CLogger             log(&cout);
  const std::string   tab("\t");

  ELevel info  = ELevel::INFO;
  ELevel debug = ELevel::DEBUG;
  ELevel warn  = ELevel::WARN;
  ELevel error = ELevel::FATAL;

//  template <typename T>
//  CLogger& operator << (ELevel& type, T const& output)
//  {
//    log << type;
//    log << output;
//    return log;
//  }

  CLoggerStrategy& operator >> (CLoggerStrategy& oStrategy, CLogger& oLog)
  {
    oLog.setStrategy(&oStrategy);
    return oStrategy;
  }
}
