#pragma once
#include <string>
#include <chrono>
namespace ChatApp
{
    namespace TimeUtils
    {
        std::string FormatTimestamp(const std::chrono::system_clock::time_point &tp);
    }
}
