#include <iostream>

#include "ChatClient.h"
#include "../Chat/utils/Input.h"
#include "../Chat/utils/Display.h"
#ifdef GetUserName
#undef GetUserName
#endif

namespace ChatApp
{
    ChatClient::ChatClient() : _connection("127.0.0.1", 16060) {}

    void ChatClient::displayLoginMenu()
    {
        _currentUser.reset();
        char option;
        do
        {
            std::cout << "\n(1) Вход\n(2) Регистрация\n(0) Выход\nВыберите действие: ";
            option = InputUtils::ReadOption();
            switch (option)
            {
            case '1':
                login();
                break;
            case '2':
                signUp();
                break;
            case '0':
                return;
            default:
                std::cout << "Выберите 1, 2 или 0.\n";
                break;
            }
        } while (!_currentUser);
    }

    void ChatClient::login()
    {
        std::string login, password;
        std::cout << "Логин: ";
        std::cin >> login;
        std::cout << "Пароль: ";
        std::cin >> password;
        std::string request = "LOGIN:" + login + ":" + password;
        std::string response = _connection.sendRequest(request);
        if (response == "OK")
        {
            _currentUser = std::make_shared<User>(login, password, login); // Имя временно = логин
        }
        else
        {
            std::cout << response << std::endl;
        }
    }

    void ChatClient::signUp()
    {
        std::string login, password, name;
        std::cout << "Логин: ";
        std::cin >> login;
        std::cout << "Пароль: ";
        std::cin >> password;
        std::cout << "Имя: ";
        std::cin >> name;
        std::string request = "REGISTER:" + login + ":" + password + ":" + name;
        std::string response = _connection.sendRequest(request);
        std::cout << response << std::endl;
        if (response == "OK")
        {
            _currentUser = std::make_shared<User>(login, password, name);
        }
    }

    void ChatClient::addMessage()
    {
        std::string to, text;
        std::cout << "Кому ('всем' для всех): ";
        std::cin >> to;
        std::cout << "Текст: ";
        std::cin.ignore();
        std::getline(std::cin, text);
        std::string request = "MESSAGE:" + _currentUser->GetUserLogin() + ":" + to + ":" + text;
        std::string response = _connection.sendRequest(request);
        std::cout << response << std::endl;
    }

    void ChatClient::displayUserMenu()
    {
        char option;
        std::cout << "Привет, " << _currentUser->GetUserName() << "!\n";
        while (_currentUser)
        {
            std::cout << "\n(1) Чат | (2) Сообщение | (0) Выход\nВыберите действие: ";
            option = InputUtils::ReadOption();
            switch (option)
            {
            case '1':
                displayChat();
                break;
            case '2':
                addMessage();
                break;
            case '0':
                _currentUser.reset();
                break;
            default:
                std::cout << "Выберите из списка.\n";
                break;
            }
        }
    }

    void ChatClient::displayChat()
    {
        // Пока заглушка, позже добавить запрос к серверу
        std::cout << "Чат пока недоступен.\n";
    }
}