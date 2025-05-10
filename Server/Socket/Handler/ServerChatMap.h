#pragma once
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include "../../../Misc/Misc.h"

/// @brief класс карты страниц чата
class ServerChatMap
{
private:
    std::set<std::string> pages;

public:
    ~ServerChatMap() = default;
    ServerChatMap();
    std::string check(std::string page, std::string command);
};

/*
Если введена команда перехода на страницу - выдать команду
Если введена не команда перехода на страницу значит это запрос от страницы - выдать страницу
*/