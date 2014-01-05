#include "error.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
LogSinkFile rogue::g_logSinkFile;
LogLevel rogue::g_logLevel = LogLevel::None;

//-----------------------------------------------------------------------------
void LogSink::Log(LogLevel level, const vector<pair<string, string> >& msg)
{
}

//-----------------------------------------------------------------------------
LogSinkFile::LogSinkFile()
    : _log(nullptr)
{
}

//-----------------------------------------------------------------------------
LogSinkFile::~LogSinkFile()
{
  if (_log)
    fclose(_log);
}

//-----------------------------------------------------------------------------
bool LogSinkFile::Open(const char* filename)
{
  _log = fopen(filename, "at");
  return !!_log;
}

//-----------------------------------------------------------------------------
void LogSinkFile::Log(LogLevel level, const vector<pair<string, string> >& msg)
{
  static char levelPrefix[] = { '-', 'D', 'I', 'W', 'E' };

  if (!_log)
    return;

  ptime now = microsec_clock::local_time();
  fprintf(_log, "[%c] %s - ", levelPrefix[(int)level], boost::posix_time::to_iso_extended_string(now).c_str());

  for (size_t i = 0; i < msg.size(); ++i)
  {
    auto& p = msg[i];
    bool last = i == msg.size() - 1;
    fprintf(_log, "%s=%s%c", p.first.c_str(), p.second.c_str(), last ? '\n' : '|');
  }
  fflush(_log);
}

//-----------------------------------------------------------------------------
LogStream::LogStream(LogSink* sink, LogLevel level)
    : _sink(sink)
    , _level(level)
{
}

//-----------------------------------------------------------------------------
LogStream::~LogStream()
{
  if (_level < g_logLevel)
    return;

  if (_sink)
    _sink->Log(_level, _output);
}

//-----------------------------------------------------------------------------
void LogStream::Append(const string& key, const string& value)
{
  if (_level < g_logLevel)
    return;

  _output.push_back(make_pair(key, value));
}

//-----------------------------------------------------------------------------
LogStream& rogue::operator<<(LogStream& s, const char* desc)
{
  std::ostringstream str;
  str << desc;
  s.Append("desc", desc);
  return s;
}


