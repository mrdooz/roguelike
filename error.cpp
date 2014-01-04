#include "error.hpp"

using namespace rogue;

void LogSink::Log(const vector<pair<string, string> >& msg)
{
  
}

LogSinkFile::LogSinkFile()
    : _log(nullptr)
{
}

LogSinkFile::~LogSinkFile()
{
  if (_log)
    fclose(_log);
}

bool LogSinkFile::Open(const char* filename)
{
  _log = fopen(filename, "at");
  return !!_log;
}

void LogSinkFile::Log(const vector<pair<string, string> >& msg)
{
  if (!_log)
    return;

  for (size_t i = 0; i < msg.size(); ++i)
  {
    auto& p = msg[i];
    bool last = i == msg.size() - 1;
    fprintf(_log, "%s=%s%c", p.first.c_str(), p.second.c_str(), last ? '\n' : '|');
  }
  fflush(_log);
}

LogStream::LogStream(LogSink* sink)
    : _sink(sink)
{
}

LogStream::~LogStream()
{
  if (_sink)
    _sink->Log(_output);
}

void LogStream::Append(const string& key, const string& value)
{
  _output.push_back(make_pair(key, value));
}

LogSinkFile rogue::g_logSinkFile;
