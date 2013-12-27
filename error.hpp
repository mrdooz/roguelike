#pragma once
#include <ostream>
#include <sstream>

#include "roguelike.hpp"

namespace rogue
{
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
    virtual void Log(const vector<pair<string, string> >& msg);
  };

  //----------------------------------------------------------------------------------
  struct LogSinkFile : public LogSink
  {
    LogSinkFile();
    ~LogSinkFile();

    bool Open(const char* filename);
    virtual void Log(const vector<pair<string, string> >& msg);

    FILE* m_log;
  };

  //----------------------------------------------------------------------------------
  struct LogSinkConsole : public LogSink
  {
  };

  //----------------------------------------------------------------------------------
  struct LogStream
  {
    LogStream(LogSink* sink);
    ~LogStream();

    void Append(const string& key, const string& value);

    vector<pair<string, string> > m_output;
    LogSink* m_sink;
  };

  template <typename T>
  LogStream& operator<<(LogStream& s, const LogObject<T>& lhs)
  {
    std::ostringstream str;
    str << lhs.value;
    s.Append(lhs.key, str.str());
    return s;
  }
  
  extern LogSinkFile g_logSinkFile;

#define LOG_DEBUG(x) \
LogStream s(&g_logSinkFile); s << x
#define LOG_INFO(x)
#define LOG_WARN(x)
#define LOG_ERROR(x)

}

