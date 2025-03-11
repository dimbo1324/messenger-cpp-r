#include <iostream>
#include "Networking.h"

class Client
{
private:
    Networking::Client networkClient_;

public:
    Client() : networkClient_("127.0.0.1", 8080)
    {
        networkClient_.connectToServer();
    }

    void run()
    {
        displayLoginMenu();
    }

private:
    void displayLoginMenu()
    {
        char option;
        do
        {
            std::cout << "\n(1) Вход\n(2) Регистрация\n(0) Выход\nВыберите действие: ";
            std::cin >> option;
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
            }
        } while (true);
    }

    void login()
    {
        std::string login, password;
        std::cout << "Логин: ";
        std::cin >> login;
        std::cout << "Пароль: ";
        std::cin >> password;
        std::string request = "LOGIN " + login + " " + password;
        networkClient_.sendMessage(request);
        std::string response = networkClient_.receiveMessage();
        std::cout << "Ответ сервера: " << response << "\n";
    }

    void signUp()
    {
        std::string login, password, name;
        std::cout << "Логин: ";
        std::cin >> login;
        std::cout << "Пароль: ";
        std::cin >> password;
        std::cout << "Имя: ";
        std::cin >> name;
        std::string request = "SIGNUP " + login + " " + password + " " + name;
        networkClient_.sendMessage(request);
        std::string response = networkClient_.receiveMessage();
        std::cout << "Ответ сервера: " << response << "\n";
    }
};

int main()
{
    Client client;
    client.run();
    return 0;
}