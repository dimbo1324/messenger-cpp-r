#include "UI.h"
#include <iostream>
#include <limits>
namespace UI
{
    char showLoginMenu()
    {
        std::cout << "[L]ogin, [Q]uit: ";
        char c;
        std::cin >> c;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return std::tolower(c);
    }
    char showUserMenu()
    {
        std::cout << "[S]end message, [O]ut (logout): ";
        char c;
        std::cin >> c;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return std::tolower(c);
    }
    std::string promptLogin()
    {
        std::string login, password;
        std::cout << "Login: ";
        std::cin >> login;
        std::cout << "Password: ";
        std::cin >> password;
        return login + " " + password;
    }
    std::string promptMessage()
    {
        std::cout << "Message: ";
        std::string msg;
        std::getline(std::cin, msg);
        return msg;
    }
}
