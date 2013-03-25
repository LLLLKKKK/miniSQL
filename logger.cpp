#include "logger.hpp"
#include <cstdio>
#include <cstdarg>

Logger* Logger::getLogger(const string& name)
{
	auto it = loggers.find(name);
	Logger* logger;
	
	if (it == loggers.end())
	{
		logger = new Logger(name);
		loggers[name] = logger;
	}
	else
	{
		logger = it->second;
	}
	
	return logger;
}

Logger::Logger(const string& name)
{
	this->name = name;	
}

Logger::~Logger()
{
	// TODO: change to shared_ptr
	for (auto i = loggers.begin(); i != loggers.end(); i++)
	{
		delete i->second;
	}
}

void Logger::log(LogLevel level, const char* format, ... )
{
	printf("%s ", name.c_str());

	switch (level)
	{
		case ERROR:
			printf("ERROR: ");
			break;
		case INFO:
			printf("INFO: ");
			break;
		case DEBUG:
			printf("DEBUG: ");
			break;
		default:
			printf("LEVEL %d: ", level);
			break;
	}
	
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

void Logger::error(const char* format, ... )
{
	printf("%s ERROR:", name.c_str());

	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}
