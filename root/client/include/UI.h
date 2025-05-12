#pragma once
#include <string>
namespace UI
{
    char showLoginMenu();
    char showUserMenu(const std::string &currentUserName);
    std::string promptLogin();
    std::string promptMessage();
}