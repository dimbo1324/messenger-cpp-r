#include "Chat.h"
#include <iostream>

namespace ChatApp
{
    Chat::Chat(const std::string &serverAddress, unsigned short serverPort)
        : client_(serverAddress, serverPort)
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
        client_.sendMessage(request);
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
        client_.sendMessage(request);
    }

    void Chat::displayChat() const
    {
        client_.sendMessage("GET_CHAT");
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
        client_.sendMessage(request);
    }
}