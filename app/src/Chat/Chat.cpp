#include "Chat.h"
#include <iostream>
#include <limits>
#include <sstream>
namespace ChatApp
{
    Chat::Chat(const std::string &serverAddress, unsigned short serverPort)
        : client_(serverAddress, serverPort, this)
    {
        client_.connectToServer();
        client_.startReceiving();
    }
    void Chat::Start()
    {
        _isActive = true;
    }
    void Chat::login()
    {
        if (_currentUser != nullptr)
        {
            std::cout << "Вы уже вошли в систему.\n";
            return;
        }
        std::string login, password;
        std::cout << "Логин: ";
        std::cin >> login;
        std::cout << "Пароль: ";
        std::cin >> password;
        std::string request = "LOGIN " + login + " " + password;
        try
        {
            if (!client_.sendMessage(request))
            {
                std::cerr << "Ошибка отправки запроса на вход\n";
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Исключение при отправке запроса на вход: " << ex.what() << std::endl;
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
        try
        {
            if (!client_.sendMessage(request))
            {
                std::cerr << "Ошибка отправки запроса на регистрацию\n";
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Исключение при отправке запроса на регистрацию: " << ex.what() << std::endl;
        }
    }
    void Chat::displayChat()
    {
        try
        {
            if (!client_.sendMessage("GET_CHAT"))
            {
                std::cerr << "Ошибка запроса чата\n";
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Исключение при запросе чата: " << ex.what() << std::endl;
        }
    }
    void Chat::displayAllUserNames()
    {
        try
        {
            if (!client_.sendMessage("GET_USERS"))
            {
                std::cerr << "Ошибка запроса списка пользователей\n";
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Исключение при запросе списка пользователей: " << ex.what() << std::endl;
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
        try
        {
            if (!client_.sendMessage(request))
            {
                std::cerr << "Ошибка отправки сообщения\n";
                return;
            }
            std::cout << "Сообщение отправлено\n";
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Исключение при отправке сообщения: " << ex.what() << std::endl;
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
            while (!(std::cin >> choice))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Неверный ввод. Попробуйте снова: ";
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
            while (!(std::cin >> choice))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Неверный ввод. Попробуйте снова: ";
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
