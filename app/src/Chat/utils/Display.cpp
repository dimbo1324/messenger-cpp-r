#include "Display.h"
#include <iostream>
#include <iomanip>
#include <ctime>
namespace ChatApp
{
namespace DisplayUtils
{
void PrintSeparator(const std::string &title)
{
if (!title.empty())
{
std::cout << "\n--- " << title << " ---\n";
}
else
{
std::cout << "\n---------------------\n";
}
}
void PrintMessage(const std::string &from, const std::string &to,
const std::string &text, const std::string &timestamp)
{
try
{
std::time_t ts = std::stoll(timestamp);
std::tm *tm_info = std::localtime(&ts);
char buffer[80];
std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
std::cout << "Время: " << buffer << "\n";
}
catch (...)
{
std::cout << "Время: " << timestamp << "\n";
}
std::cout << "Сообщение от " << from << " для " << to << ":\n"
<< text << "\n\n";
}
}
}
