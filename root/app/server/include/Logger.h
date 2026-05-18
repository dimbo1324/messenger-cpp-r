#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>
#include <string>

enum class LogLevel
{
    Info,
    Warn,
    Error
};

class Logger
{
public:
    static Logger &getInstance();

    void configure(const std::string &path);
    void log(LogLevel level, const std::string &message);
    void info(const std::string &message);
    void warn(const std::string &message);
    void error(const std::string &message);

    void log(const std::string &message);

private:
    Logger();
    ~Logger();
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    std::ofstream logFile_;
    std::mutex logMutex_;
    bool stderrFallback_{true};
};

#endif
