#include "error.hpp"

using namespace rogue;

void LogSink::Log(const vector<pair<string, string> >& msg)
{
  
}

LogSinkFile::LogSinkFile()
    : m_log(nullptr)
{
}

LogSinkFile::~LogSinkFile()
{
  if (m_log)
    fclose(m_log);
}

bool LogSinkFile::Open(const char* filename)
{
  m_log = fopen(filename, "at");
  return !!m_log;
}

void LogSinkFile::Log(const vector<pair<string, string> >& msg)
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

LogStream::LogStream(LogSink* sink)
    : m_sink(sink)
{
}

LogStream::~LogStream()
{
  if (m_sink)
    m_sink->Log(m_output);
}

void LogStream::Append(const string& key, const string& value)
{
  m_output.push_back(make_pair(key, value));
}

LogSinkFile rogue::g_logSinkFile;
