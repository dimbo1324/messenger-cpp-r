#include <limits>
#include "Input.h"
namespace ChatApp
{
namespace InputUtils
{
char ReadOption()
{
char option;
std::cin >> option;
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
return option;
}
std::string ReadLine()
{
std::string line;
std::getline(std::cin, line);
return line;
}
}
}
