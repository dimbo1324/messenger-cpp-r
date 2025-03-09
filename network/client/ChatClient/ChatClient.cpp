#include <iostream>
#include <thread>
#include "ChatClient.h"
#include "../Chat/utils/Input.h"
#include "../Chat/utils/Display.h"
#include <sstream>
#include <stdexcept>
#include <algorithm>
#ifdef GetUserName
#undef GetUserName
#endif

#include <nlohmann/json.hpp>
using json = nlohmann::json;

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
        std::string login, password, username;
        std::cout << "Логин: ";
        std::getline(std::cin, login);
        std::cout << "Пароль: ";
        std::getline(std::cin, password);
        std::cout << "Имя для отображения: ";
        std::getline(std::cin, username);

        std::string request = "LOGIN:" + login + ":" + password;
        try
        {
            if (!_connection.isConnected())
            {
                throw std::runtime_error("Нет соединения с сервером");
            }
            std::string response = _connection.sendRequest(request);
            if (response == "OK")
            {
                _currentUser = std::make_shared<User>(login, password, username);
            }
            else
            {
                std::cout << "Ошибка: " << response << std::endl;
            }
        }
        catch (const std::exception &ex)
        {
            std::cout << "Ошибка при входе: " << ex.what() << std::endl;
        }
    }

    void ChatClient::signUp()
    {
        std::string login, password, name;
        std::cout << "Логин: ";
        std::getline(std::cin, login);
        std::cout << "Пароль: ";
        std::getline(std::cin, password);
        std::cout << "Имя: ";
        std::getline(std::cin, name);

        std::string request = "REGISTER:" + login + ":" + password + ":" + name;
        try
        {
            if (!_connection.isConnected())
            {
                throw std::runtime_error("Нет соединения с сервером");
            }
            std::string response = _connection.sendRequest(request);
            std::cout << response << std::endl;
            if (response == "OK")
            {
                _currentUser = std::make_shared<User>(login, password, name);
            }
        }
        catch (const std::exception &ex)
        {
            std::cout << "Ошибка при регистрации: " << ex.what() << std::endl;
        }
    }

    void ChatClient::addMessage()
    {
        std::string to, text;
        std::cout << "Кому ('всем' для всех): ";
        std::getline(std::cin, to);
        std::cout << "Текст: ";
        std::getline(std::cin, text);

        std::string request = "MESSAGE:" + _currentUser->GetUserLogin() + ":" + to + ":" + text;
        try
        {
            if (!_connection.isConnected())
            {
                throw std::runtime_error("Нет соединения с сервером");
            }
            std::string response = _connection.sendRequest(request);
            std::cout << response << std::endl;
        }
        catch (const std::exception &ex)
        {
            std::cout << "Ошибка при отправке сообщения: " << ex.what() << std::endl;
        }
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
        std::string request = "GET_MESSAGES";
        try
        {
            if (!_connection.isConnected())
            {
                throw std::runtime_error("Нет соединения с сервером");
            }
            std::string response = _connection.sendRequest(request);
            try
            {
                auto jsonResponse = json::parse(response);
                std::cout << "Сообщения:\n";
                for (auto &message : jsonResponse["messages"])
                {
                    std::cout << message["from"].get<std::string>() << ": "
                              << message["text"].get<std::string>() << "\n";
                }
            }
            catch (json::parse_error &)
            {
                std::cout << "Сообщения (не JSON):\n"
                          << response << std::endl;
            }
        }
        catch (const std::exception &ex)
        {
            std::cout << "Ошибка при получении сообщений: " << ex.what() << std::endl;
        }
    }
}
