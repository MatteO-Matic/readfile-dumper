#pragma once
#include <string>

using namespace std;
class Logger
{
  public:
    void Log( const char * format, ... );
    void Log(const std::string& sMessage);
    static Logger* getLogger();

  private:
    Logger();
    Logger( const Logger&){};
    Logger& operator=(const Logger& ){ return *this;};
    Logger& operator<<(const string& sMessage );
    static Logger* m_self;
};
