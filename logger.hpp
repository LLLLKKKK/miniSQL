
#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <map>
#include <string>

using std::map;
using std::string;

class Logger
{

private:
	static map<string, Logger*> loggers;
	string name;

	Logger(const string&);
	~Logger();
	
public:

	enum LogLevel
	{
		ERROR,
		DEBUG,
		INFO
	};
	
	static Logger* getLogger(const string& name);
	void log(LogLevel level, const char* format, ... );
	void error(const char* format, ... );

};

#endif
