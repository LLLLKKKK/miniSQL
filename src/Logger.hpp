
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <map>
#include <memory>
#include <string>

namespace miniSQL {

class Logger;
typedef std::shared_ptr<Logger> LoggerPtr;

class Logger {
private:
    Logger(const std::string&);

private:
    void printfCurTime();
    static std::map<std::string, LoggerPtr > loggers;
    static const char*level_str_ [];
    static const int MAX_MESSAGE_SIZE;

    std::string name;

public:
    enum LogLevel {
        ERROR, WARN, INFO, DEBUG, TRACE
    };
    
public:
    static LoggerPtr getLogger(const std::string& name);
    void log(LogLevel level, const char* format, ... );
    void log(LogLevel level, const char* file, const int line, const char* func,
             const char* format, ... );
};


#define _NO_VERBOSE_

#ifndef _NO_LOG_

#define LOG(logger, level, format, args...)                             \
    logger->log(level, __FILE__, __LINE__, __FUNCTION__, format, ##args)

#define LOG_ERROR(logger, format, args...)                              \
    logger->log(Logger::ERROR, __FILE__, __LINE__, __FUNCTION__, format, ##args)

#define LOG_WARN(logger, format, args...) \
    logger->log(Logger::WARN, __FILE__, __LINE__, __FUNCTION__, format, ##args)

#define LOG_INFO(logger, format, args...) \
    logger->log(Logger::INFO, __FILE__, __LINE__, __FUNCTION__, format, ##args)

#define LOG_TRACE(logger, format, args...) \
    logger->log(Logger::TRACE, __FILE__, __LINE__, __FUNCTION__, format, ##args)

#define LOG_DEBUG(logger, format, args...) \
    logger->log(Logger::DEBUG, __FILE__, __LINE__, __FUNCTION__, format, ##args)

#else

#define LOG
#define LOG_ERROR
#define LOG_WARN
#define LOG_INFO
#define LOG_TRACE
#define LOG_DEBUG

#endif

#define DECLARE_LOGGER() static LoggerPtr _logger
#define SETUP_LOGGER(c)                                    \
    LoggerPtr c::_logger = Logger::getLogger(#c)

#define MINISQL_LOG(level, format, args...)     \
    LOG(_logger, level, format, ##args)

#define MINISQL_LOG_ERROR(format, args...)      \
    LOG_ERROR(_logger, format, ##args)

#define MINISQL_LOG_WARN(format, args...)      \
    LOG_WARN(_logger, format, ##args)

#define MINISQL_LOG_INFO(format, args...)      \
    LOG_INFO(_logger, format, ##args)

#define MINISQL_LOG_DEBUG(format, args...)      \
    LOG_DEBUG(_logger, format, ##args)

#define MINISQL_LOG_TRACE(format, args...)      \
    LOG_TRACE(_logger, format, ##args)

}

#endif
