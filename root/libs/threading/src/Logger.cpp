#include "threading/Logger.h"
#include <iostream>
#include "threading_config.h"
namespace threading
{
    Logger &Logger::instance()
    {
        static Logger inst;
        return inst;
    }
    void Logger::log(const std::string &tag, const std::string &message)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        std::cout << "[" << tag << "] " << message << std::endl;
    }
}