#include "Logger.h" // обязательно первым, чтобы подтянуть объявление класса
#include <chrono>   // для std::chrono::system_clock
#include <ctime>    // для std::localtime
#include <iomanip>  // для std::put_time
#include <sstream>  // для std::ostringstream

Logger::Logger()
{
    logFile.open("server.log", std::ios::app);
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

    // Формируем метку времени
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    std::ostringstream ts;
    ts << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S] ");

    logFile << ts.str() << message << std::endl;
}
