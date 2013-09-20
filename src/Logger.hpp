
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <map>
#include <memory>
#include <string>

#define LoggerPtr std::shared_ptr<Logger>

class Logger
{

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
    static LOGGER_PTR getLogger(const std::string& name);
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
#define TRACE
#define DEBUG

#endif

#define DECLARE_LOG() static LoggerPtr _logger
#define SETUP_LOG(c)                                    \
    static LoggerPtr c::_logger = Logger::getLogger(#c)

#define MINISQL_LOG(level, format, args...)     \
    LOG(_logger, level, format, ##args)

#endif
