#include "UI.h"
#include <iostream>
#include <limits>
#include <string>
#include <cctype>
namespace UI
{
    char showLoginMenu()
    {
        std::cout << "[L]ogin, [Q]uit: ";
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
        std::cout << "[" << currentUserName << "] [S]end message, [O]ut (logout): ";
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
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return "";
        }
        std::cout << "Password: ";
        if (!(std::cin >> password))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return "";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (login.empty())
            return "";
        return login + " " + password;
    }
    std::string promptMessage()
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
