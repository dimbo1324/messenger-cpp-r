#include "Chat.h"
#include <iostream>
#include <limits>
#include <sstream>

namespace ChatApp
{
    Chat::Chat(const std::string &serverAddress, unsigned short serverPort)
        : client_(serverAddress, serverPort, this)
    {
        if (!client_.connectToServer())
        {
            throw std::runtime_error("Не удалось подключиться к серверу");
        }
        client_.startReceiving();
    }

    void Chat::Start()
    {
        _isActive = true;
    }

    void Chat::login()
    {
        std::string login, password;
        std::cout << "Логин: ";
        std::cin >> login;
        std::cout << "Пароль: ";
        std::cin >> password;
        std::string request = "LOGIN " + login + " " + password;
        if (!client_.sendMessage(request))
        {
            std::cerr << "Ошибка отправки запроса на вход\n";
        }
    }

    void Chat::signUp()
    {
        std::string login, password, name;
        std::cout << "Придумайте логин: ";
        std::cin >> login;
        std::cout << "Придумайте пароль: ";
        std::cin >> password;
        std::cout << "Ваше имя: ";
        std::cin >> name;
        std::string request = "SIGNUP " + login + " " + password + " " + name;
        if (!client_.sendMessage(request))
        {
            std::cerr << "Ошибка отправки запроса на регистрацию\n";
        }
    }

    void Chat::displayChat()
    {
        if (!client_.sendMessage("GET_CHAT"))
        {
            std::cerr << "Ошибка запроса чата\n";
        }
    }

    void Chat::displayAllUserNames()
    {
        if (!client_.sendMessage("GET_USERS"))
        {
            std::cerr << "Ошибка запроса списка пользователей\n";
        }
    }

    void Chat::addMessage()
    {
        std::string recipient, text;
        std::cout << "Введите имя получателя или 'всем': ";
        std::cin >> recipient;
        std::cout << "Текст сообщения: ";
        std::cin.ignore();
        std::getline(std::cin, text);
        std::string request = "SEND " + recipient + " " + text;
        if (!client_.sendMessage(request))
        {
            std::cerr << "Ошибка отправки сообщения\n";
        }
    }

    void Chat::displayLoginMenu()
    {
        while (_currentUser == nullptr && _isActive)
        {
            std::cout << "\n--- Меню входа ---\n";
            std::cout << "1. Вход\n";
            std::cout << "2. Регистрация\n";
            std::cout << "3. Выход\n";
            std::cout << "Выберите опцию: ";

            int choice;
            std::cin >> choice;

            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Неверный ввод. Попробуйте снова.\n";
                continue;
            }

            switch (choice)
            {
            case 1:
                login();
                break;
            case 2:
                signUp();
                break;
            case 3:
                std::cout << "Выход из приложения.\n";
                _isActive = false;
                return;
            default:
                std::cout << "Неверный выбор. Попробуйте снова.\n";
            }
        }
    }

    void Chat::displayUserMenu()
    {
        while (_isActive && _currentUser != nullptr)
        {
            std::cout << "\n--- Меню пользователя ---\n";
            std::cout << "1. Просмотр чата\n";
            std::cout << "2. Отправить сообщение\n";
            std::cout << "3. Выход\n";
            std::cout << "Выберите опцию: ";

            int choice;
            std::cin >> choice;

            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Неверный ввод. Попробуйте снова.\n";
                continue;
            }

            switch (choice)
            {
            case 1:
                displayChat();
                break;
            case 2:
                addMessage();
                break;
            case 3:
                std::cout << "Выход из аккаунта.\n";
                _currentUser = nullptr;
                return;
            default:
                std::cout << "Неверный выбор. Попробуйте снова.\n";
            }
        }
    }
}
