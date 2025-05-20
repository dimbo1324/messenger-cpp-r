#include "Logger.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

Logger::Logger()
{
    logFile.open("../logs/user_activity.log", std::ios::app);
    if (!logFile.is_open())
    {
        throw std::runtime_error("Не удалось открыть файл логов");
    }
}

Logger::~Logger()
{
    if (logFile.is_open())
        logFile.close();
}

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::log(const std::string &message)
{
    std::lock_guard<std::mutex> guard(logMutex);
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream ts;
    ts << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S] ");
    logFile << ts.str() << message << std::endl;
}