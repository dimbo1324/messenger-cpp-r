#pragma once
#include <string>
#include <mutex>
namespace threading
{
    class Logger
    {
    public:
        static Logger &instance();
        void log(const std::string &tag, const std::string &message);

    private:
        Logger() = default;
        ~Logger() = default;
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;
        std::mutex mtx_;
    };
}