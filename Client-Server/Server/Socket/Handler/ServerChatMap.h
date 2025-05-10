#pragma once
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include "../../../Misc/Misc.h"
class ServerChatMap
{
private:
    std::set<std::string> pages;

public:
    ~ServerChatMap() = default;
    ServerChatMap();
    std::string check(std::string page, std::string command);
};
