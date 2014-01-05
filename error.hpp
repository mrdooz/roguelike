#pragma once
#include "utils.hpp"

namespace rogue
{
  // Used to determine log cutoff, ie the lowest log level that will be output
  enum class LogLevel
  {
    None,
    Debug,
    Info,
    Warning,
    Error,
  };

  //----------------------------------------------------------------------------------
  template <typename T>
  struct LogObject
  {
    LogObject(const string& key, const T& value) : key(key), value(value) {}
    string key;
    T value;
  };

  //----------------------------------------------------------------------------------
  template <typename T>
  LogObject<T> LogKeyValue(const string& key, const T& value)
  {
    return LogObject<T>(key, value);
  }

  //----------------------------------------------------------------------------------
  struct LogSink
  {
    virtual void Log(LogLevel level, const vector<pair<string, string> >& msg) = 0;
  };

  //----------------------------------------------------------------------------------
  struct LogSinkFile : public LogSink
  {
    LogSinkFile();
    ~LogSinkFile();

    bool Open(const char* filename);
    virtual void Log(LogLevel level, const vector<pair<string, string> >& msg);

    FILE* _log;
  };

  //----------------------------------------------------------------------------------
  struct LogSinkConsole : public LogSink
  {
  };

  //----------------------------------------------------------------------------------
  struct LogStream
  {
    LogStream(LogSink* sink, LogLevel level);
    ~LogStream();

    void Append(const string& key, const string& value);

    vector<pair<string, string> > _output;
    LogSink* _sink;
    LogLevel _level;
  };

  template <typename T>
  LogStream& operator<<(LogStream& s, const LogObject<T>& lhs)
  {
    std::ostringstream str;
    str << lhs.value;
    s.Append(lhs.key, str.str());
    return s;
  }

  LogStream& operator<<(LogStream& s, const char* desc);
  
  extern LogSinkFile g_logSinkFile;
  extern LogLevel g_logLevel;

#define LOG_DEBUG(x) \
  LogStream GEN_NAME(s, __LINE__)(&g_logSinkFile, LogLevel::Debug); GEN_NAME(s, __LINE__) << x
#define LOG_INFO(x) \
  LogStream GEN_NAME(s, __LINE__)(&g_logSinkFile, LogLevel::Info); GEN_NAME(s, __LINE__) << x
#define LOG_WARN(x) \
  LogStream GEN_NAME(s, __LINE__)(&g_logSinkFile, LogLevel::Warning); GEN_NAME(s, __LINE__) << x
#define LOG_ERROR(x)  \
  LogStream GEN_NAME(s, __LINE__)(&g_logSinkFile, LogLevel::Error); GEN_NAME(s, __LINE__) << x
}

