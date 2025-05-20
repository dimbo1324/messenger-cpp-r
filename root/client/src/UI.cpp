#include "UI.h"
#include <iostream>
#include <limits>
#include <string>
#include <cctype>

namespace UI
{
    char showInitialMenu()
    {
        std::cout << "Выберите действие:\n";
        std::cout << "[L]ogin - Вход\n";
        std::cout << "[R]egister - Регистрация\n";
        std::cout << "[Q]uit - Выход\n";
        std::cout << "Ваш выбор: ";
        char c;
        if (!(std::cin >> c))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return '\0';
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return static_cast<char>(std::tolower(c));
    }

    char showUserMenu(const std::string &currentUserName)
    {
        std::cout << "Пользователь: " << currentUserName << "\n";
        std::cout << "Выберите действие:\n";
        std::cout << "[L]ist users - Список онлайн пользователей\n";
        std::cout << "[I]nbox - Входящие сообщения\n";
        std::cout << "[S]end - Отправить сообщение\n";
        std::cout << "[H]istory - История сообщений\n";
        std::cout << "[O]ut - Выйти из аккаунта\n";
        std::cout << "Ваш выбор: ";
        char c;
        if (!(std::cin >> c))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return '\0';
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return static_cast<char>(std::tolower(c));
    }

    std::string promptLogin()
    {
        std::string login, password;
        std::cout << "Введите логин: ";
        if (!(std::cin >> login) || login.empty())
        {
            std::cout << "Логин не может быть пустым.\n";
            return "";
        }
        std::cout << "Введите пароль: ";
        if (!(std::cin >> password) || password.empty())
        {
            std::cout << "Пароль не может быть пустым.\n";
            return "";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return login + " " + password;
    }

    std::string promptRegistration()
    {
        std::string login, password;
        std::cout << "Введите новый логин: ";
        if (!(std::cin >> login) || login.empty())
        {
            std::cout << "Логин не может быть пустым.\n";
            return "";
        }
        std::cout << "Введите новый пароль: ";
        if (!(std::cin >> password) || password.empty())
        {
            std::cout << "Пароль не может быть пустым.\n";
            return "";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return login + " " + password;
    }

    std::string promptTargetUser()
    {
        std::cout << "Введите имя пользователя: ";
        std::string u;
        if (!(std::cin >> u) || u.empty())
        {
            std::cout << "Имя пользователя не может быть пустым.\n";
            return "";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return u;
    }

    std::string promptMessage()
    {
        std::cout << "Введите сообщение: ";
        std::string msg;
        if (!std::getline(std::cin, msg) || msg.empty())
        {
            std::cout << "Сообщение не может быть пустым.\n";
            return "";
        }
        return msg;
    }
}