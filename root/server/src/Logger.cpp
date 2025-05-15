#include "Logger.h"
#include <iostream>

namespace server
{

    Logger &Logger::instance()
    {
        static Logger inst;
        return inst;
    }

    void Logger::log(const std::string &tag, const std::string &message)
    {
        std::lock_guard<std::mutex> lk(mtx_);
        std::cout << "[" << tag << "] " << message << std::endl;
    }

}
