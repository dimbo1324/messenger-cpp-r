#pragma once
#include <string>

namespace ChatApp
{
    namespace DisplayUtils
    {
        void PrintSeparator(const std::string &title = "");

        void PrintMessage(const std::string &from, const std::string &to,
                          const std::string &text, const std::string &timestamp);
    }
}
