#pragma once

#include <string>

struct Message
{
    std::string from;
    std::string to;
    std::string text;
    std::string timestamp;
};