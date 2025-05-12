#include "Time.h"
#include <sstream>
#include <iomanip>
#include <ctime>
namespace ChatApp
{
namespace TimeUtils
{
std::string FormatTimestamp(const std::chrono::system_clock::time_point &tp)
{
std::time_t time = std::chrono::system_clock::to_time_t(tp);
std::tm tm;
#ifdef _WIN32
localtime_s(&tm, &time);
#else
localtime_r(&time, &tm);
#endif
std::ostringstream oss;
oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
return oss.str();
}
}
}
