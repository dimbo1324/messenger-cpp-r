#pragma once

#include <string>

std::string getOSInfo();
std::string getProcessInfo();

#if defined(_WIN32)
void initConsoleOutput();
#endif
