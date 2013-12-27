#include "precompiled.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "roguelike.hpp"
#include "error.cpp"

using namespace rogue;

#if 0
#include <ostream>
#include <sstream>


template <typename T>
struct LogObject
{
  LogObject(const string& key, const T& value) : key(key), value(value) {}
  string key;
  T value;
};

template <typename T>
LogObject<T> LogKeyValue(const string& key, const T& value)
{
  return LogObject<T>(key, value);
}

struct LogSink
{
  virtual void Log(const vector<pair<string, string> >& msg)
  {

  }
};

struct LogSinkFile : public LogSink
{
  LogSinkFile()
      : m_log(nullptr)
  {
  }

  ~LogSinkFile()
  {
    if (m_log)
      fclose(m_log);
  }

  bool Open(const char* filename)
  {
    m_log = fopen(filename, "wt");
    return !!m_log;
  }

  virtual void Log(const vector<pair<string, string> >& msg)
  {
    if (!m_log)
      return;

    for (size_t i = 0; i < msg.size(); ++i)
    {
      auto& p = msg[i];
      bool last = i == msg.size() - 1;
      fprintf(m_log, "%s=%s%c", p.first.c_str(), p.second.c_str(), last ? '\n' : '|');
    }
    fflush(m_log);
  }

  FILE* m_log;
};

struct LogSinkConsole : public LogSink
{

};

struct LogStream
{
  LogStream(LogSink* sink)
      : m_sink(sink)
  {
  }

  ~LogStream()
  {
    if (m_sink)
      m_sink->Log(m_output);
  }

  void Append(const string& key, const string& value)
  {
    m_output.push_back(make_pair(key, value));
  }

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

LogSinkFile g_logSinkFile;

#define LOG_DEBUG(x) \
  LogStream s(&g_logSinkFile); s << x
#define LOG_INFO(x)
#define LOG_WARN(x)
#define LOG_ERROR(x)
#endif

int main()
{
  if (!g_logSinkFile.Open("rogue.log"))
    return 1;

  LOG_DEBUG(LogKeyValue("tjong", 10) << LogKeyValue("test", 10.0f));
//  s << LogKeyValue("tjong", 10) << LogKeyValue("test", 10.0f);
//  s << LogKeyValue("val", false);
//    << LogKeyValue("tjong2", 10.0f);

  srand(1337);

  if (!Game::create())
    return 1;

  int res = GAME.run();

  GAME.close();

  if (res)
  {
#ifdef _WIN32
    while (!isBitSet(GetAsyncKeyState(VK_ESCAPE), 15))
      ;
#endif
  }

  return res;
}
