#pragma once
#include <string>
namespace UI
{
    char showInitialMenu();
    char showUserMenu(const std::string &currentUserName);
    std::string promptLogin();
    std::string promptRegistration();
    std::string promptTargetUser();
    std::string promptMessage();
}
