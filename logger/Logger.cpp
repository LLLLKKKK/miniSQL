#include "Logger.hpp"
#include <cstdio>
#include <cstdarg>
#include <cassert>

using std::string;

namespace miniSQL {

std::map<string, LoggerPtr> Logger::loggers;
const int Logger::MAX_MESSAGE_SIZE = 1024;

const char* Logger::level_str_[] = {
    "ERROR", "WARN", "INFO", "DEBUG", "TRACE"
};


LoggerPtr Logger::getLogger(const string& name) {
    auto it = loggers.find(name);
    LoggerPtr logger;

    if (it == loggers.end()) {
        logger = LoggerPtr(new Logger(name));
        loggers.emplace(name, logger);
    }
    else {
        logger = it->second;
    }

    assert(logger);
    return logger;
}

Logger::Logger(const string& name) : _name(name) { }

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

}
