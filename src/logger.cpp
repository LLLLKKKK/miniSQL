#include "logger.hpp"
#include <cstdio>
#include <cstdarg>

using std::string;

std::map<string, LOGGER_PTR > Logger::loggers;
const int Logger::MAX_MESSAGE_SIZE = 1024;

const char* Logger::level_str_[] = {
    "ERROR", "WARN", "INFO", "DEBUG", "TRACE"
};


LOGGER_PTR Logger::getLogger(const string& name) {
    auto it = loggers.find(name);
    LOGGER_PTR logger;
    
    if (it == loggers.end()) {
        logger = LOGGER_PTR(new Logger(name));
        loggers[name] = logger;
    }
    else {
        logger = it->second;
    }
    
    return logger;
}

Logger::Logger(const string& name) {
    this->name = name;  
}

void Logger::printfCurTime() {
    time_t n = time(NULL);
    struct tm * p = localtime(&n);
    printf("[%04d-%02d-%02d %02d:%02d:%02d]",
           p->tm_year + 1900, p->tm_mon + 1, p->tm_mday,
           p->tm_hour, p->tm_min, p->tm_sec);
}

void Logger::log(LogLevel level, const char* format, ... ) {
    printfCurTime();
    printf(" %s: ", level_str_[level]);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    putchar('\n');
}

void Logger::log(LogLevel level, const char* file, const int line, const char* func,
                 const char* format, ...) { 
    printfCurTime();
    printf(" %s:%d %s() %s: ", file, line, func, level_str_[level]);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    putchar('\n');
}
