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
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return '\0';
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    char showUserMenu(const std::string &currentUserName)
    {
        std::cout << "[" << currentUserName << "] "
                  << "[L]ist users, [S]end message, [H]istory, [O]ut: ";
        char c;
        if (!(std::cin >> c))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return '\0';
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    std::string promptLogin()
    {
        std::string login, password;
        std::cout << "Login: ";
        if (!(std::cin >> login))
        {
            std::cin.clear();
            return "";
        }
        std::cout << "Password: ";
        if (!(std::cin >> password))
        {
            std::cin.clear();
            return "";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return login + " " + password;
    }

    std::string promptRegistration()
    {
        std::string login, password;
        std::cout << "New login: ";
        if (!(std::cin >> login))
        {
            std::cin.clear();
            return "";
        }
        std::cout << "New password: ";
        if (!(std::cin >> password))
        {
            std::cin.clear();
            return "";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return login + " " + password;
    }

    std::string promptTargetUser()
    {
        std::cout << "User name: ";
        std::string u;
        if (!(std::cin >> u))
        {
            std::cin.clear();
            return "";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
