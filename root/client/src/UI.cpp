#include "UI.h"
#include <iostream>
#include <limits>
#include <string>
#include <cctype>
namespace UI
{
    char showInitialMenu()
    {
        std::cout << "[L]ogin, [R]egister, [Q]uit: ";
        char c;
        if (!(std::cin >> c))
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            return '\0';
        }
        std::cin.ignore(INT_MAX, '\n');
        return static_cast<char>(std::tolower(c));
    }
    char showUserMenu(const std::string &currentUserName)
    {
        std::cout << "[" << currentUserName << "] "
                  << "[L]ist users, [I]nbox, [S]end, [H]istory, [O]ut: ";
        char c;
        if (!(std::cin >> c))
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            return '\0';
        }
        std::cin.ignore(INT_MAX, '\n');
        return static_cast<char>(std::tolower(c));
    }
    std::string promptLogin()
    {
        std::string login, password;
        std::cout << "Login: ";
        if (!(std::cin >> login))
            return "";
        std::cout << "Password: ";
        if (!(std::cin >> password))
            return "";
        std::cin.ignore(INT_MAX, '\n');
        return login + " " + password;
    }
    std::string promptRegistration()
    {
        std::string login, password;
        std::cout << "New login: ";
        if (!(std::cin >> login))
            return "";
        std::cout << "New password: ";
        if (!(std::cin >> password))
            return "";
        std::cin.ignore(INT_MAX, '\n');
        return login + " " + password;
    }
    std::string promptTargetUser()
    {
        std::cout << "User name: ";
        std::string u;
        if (!(std::cin >> u))
            return "";
        std::cin.ignore(INT_MAX, '\n');
        return u;
    }
    std::string promptMessage()
    {
        std::cout << "Message: ";
        std::string msg;
        if (!std::getline(std::cin, msg))
        {
            std::cin.clear();
            return "";
        }
        return msg;
    }
}
