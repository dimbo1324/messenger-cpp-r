#include "Client.h"
#include "UI.h"
#include "tcp/SocketFactory.h"
#include "tcp_config.h"
#include <iostream>
#include <vector>
#include <sstream>
Client::Client(const std::string &host, unsigned short port)
    : serverHost_(host), serverPort_(port), socket_(nullptr), running_(false), currentUser_("") {}
Client::~Client()
{
    running_ = false;
    if (socket_)
    {
        socket_->close();
    }
    if (recvThread_.joinable())
    {
        recvThread_.join();
    }
    std::cout << "Клиент завершил работу.\n";
}
void Client::connectToServer()
{
    socket_ = tcp::createSocket();
    if (!socket_)
    {
        std::cerr << "Ошибка: Не удалось создать объект сокета.\n";
        running_ = false;
        return;
    }
    std::cout << "Попытка подключения к " << serverHost_ << ":" << serverPort_ << "...\n";
    if (!socket_->connect(serverHost_, serverPort_))
    {
        std::cerr << "Ошибка: Не удалось подключиться к серверу.\n";
        socket_.reset();
        running_ = false;
        return;
    }
    std::cout << "Успешно подключено к серверу.\n";
    running_ = true;
}
void Client::login()
{
    if (!socket_ || !running_)
    {
        std::cerr << "Не подключено к серверу. Вход невозможен.\n";
        return;
    }
    auto creds_str = UI::promptLogin();
    if (creds_str.empty())
    {
        std::cout << "Вход отменен.\n";
        return;
    }
    std::string login_packet = "LOGIN " + creds_str + "\n";
    std::size_t sent_bytes = socket_->send(login_packet.c_str(), login_packet.length());
    if (sent_bytes != login_packet.length())
    {
        std::cerr << "Ошибка отправки данных для входа.\n";
        running_ = false;
        return;
    }
    char buffer[TCP_BUFFER_SIZE];
    std::size_t received = socket_->receive(buffer, TCP_BUFFER_SIZE - 1);
    if (received > 0)
    {
        buffer[received] = '\0';
        std::string response(buffer);
        if (response.find("LOGIN_OK") != std::string::npos)
        {
            size_t space_pos = creds_str.find(' ');
            if (space_pos != std::string::npos)
            {
                currentUser_ = creds_str.substr(0, space_pos);
            }
            else
            {
                currentUser_ = creds_str;
            }
            std::cout << "Вход успешен для пользователя: " << currentUser_ << "\n";
        }
        else
        {
            std::cerr << "Ошибка входа: " << response << "\n";
        }
    }
    else
    {
        std::cerr << "Ошибка получения ответа от сервера.\n";
        running_ = false;
    }
}
void Client::sendMessage()
{
    if (!socket_ || !running_ || currentUser_.empty())
    {
        std::cerr << "Не подключено к серверу или не выполнен вход.\n";
        return;
    }
    std::string message = UI::promptMessage();
    if (message.empty())
    {
        std::cout << "Сообщение не отправлено.\n";
        return;
    }
    std::string packet = "MESSAGE " + currentUser_ + " " + message + "\n";
    std::size_t sent_bytes = socket_->send(packet.c_str(), packet.length());
    if (sent_bytes != packet.length())
    {
        std::cerr << "Ошибка отправки сообщения.\n";
        running_ = false;
    }
    else
    {
        std::cout << "Сообщение отправлено.\n";
    }
}
void Client::receiveLoop()
{
    while (running_)
    {
        char buffer[TCP_BUFFER_SIZE];
        std::size_t received = socket_->receive(buffer, TCP_BUFFER_SIZE - 1);
        if (received > 0)
        {
            buffer[received] = '\0';
            std::cout << "Получено: " << buffer << "\n";
        }
        else if (received == 0)
        {
            std::cout << "Сервер закрыл соединение.\n";
            running_ = false;
            break;
        }
        else
        {
            std::cerr << "Ошибка при получении данных.\n";
            running_ = false;
            break;
        }
    }
}
void Client::run()
{
    connectToServer();
    if (!running_)
    {
        return;
    }
    recvThread_ = std::thread(&Client::receiveLoop, this);
    while (running_)
    {
        if (currentUser_.empty())
        {
            char cmd = UI::showLoginMenu();
            if (cmd == 'l')
            {
                login();
            }
            else if (cmd == 'q')
            {
                running_ = false;
            }
            else
            {
                std::cout << "Неверный ввод.\n";
            }
        }
        else
        {
            char cmd = UI::showUserMenu(currentUser_);
            if (cmd == 's')
            {
                sendMessage();
            }
            else if (cmd == 'o')
            {
                currentUser_ = "";
                std::cout << "Выход из аккаунта.\n";
            }
            else
            {
                std::cout << "Неверный ввод.\n";
            }
        }
    }
    if (recvThread_.joinable())
    {
        recvThread_.join();
    }
}