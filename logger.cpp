#include "logger.h"
#include <stdarg.h>
#include <windows.h>

using namespace std;

Logger* Logger::m_self = NULL;

Logger::Logger()
{

}

Logger* Logger::getLogger()
{
  if(m_self == NULL)
  {
    m_self = new Logger();
  }
  return m_self;
}

void Logger::Log( const char * format, ... )
{
  char sMessage[256];
  va_list args;
  va_start (args, format);
  vsprintf (sMessage,format, args);
  OutputDebugString(sMessage);
  va_end (args);
}

void Logger::Log( const string& sMessage )
{
  OutputDebugString(sMessage.c_str());
}

Logger& Logger::operator<<(const string& sMessage )
{
  Logger::Log(sMessage);
  return *this;
}
