#include "Logger.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace
{
    const char *toString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warn:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
        }
        return "INFO";
    }

    std::tm localTime(std::time_t value)
    {
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &value);
#else
        localtime_r(&value, &tm);
#endif
        return tm;
    }
}

Logger::Logger() = default;

Logger::~Logger()
{
    if (logFile_.is_open())
    {
        logFile_.close();
    }
}

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::configure(const std::string &path)
{
    std::lock_guard<std::mutex> guard(logMutex_);
    if (logFile_.is_open())
    {
        logFile_.close();
    }

    try
    {
        const std::filesystem::path logPath(path);
        if (logPath.has_parent_path())
        {
            std::filesystem::create_directories(logPath.parent_path());
        }
        logFile_.open(logPath, std::ios::app);
        stderrFallback_ = !logFile_.is_open();
    }
    catch (const std::exception &e)
    {
        stderrFallback_ = true;
        std::cerr << "Logger fallback to stderr: " << e.what() << std::endl;
    }
}

void Logger::log(LogLevel level, const std::string &message)
{
    std::lock_guard<std::mutex> guard(logMutex_);
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    const std::tm tm = localTime(t);

    std::ostringstream line;
    line << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
         << " [" << toString(level) << "] " << message;

    if (!stderrFallback_ && logFile_.is_open())
    {
        logFile_ << line.str() << std::endl;
    }
    else
    {
        std::cerr << line.str() << std::endl;
    }
}

void Logger::info(const std::string &message)
{
    log(LogLevel::Info, message);
}

void Logger::warn(const std::string &message)
{
    log(LogLevel::Warn, message);
}

void Logger::error(const std::string &message)
{
    log(LogLevel::Error, message);
}

void Logger::log(const std::string &message)
{
    info(message);
}
