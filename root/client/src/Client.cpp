#include "Client.h"
#include "UI.h"
#include "tcp/SocketFactory.h"
#include "tcp_config.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>
Client::Client(const std::string &host, unsigned short port)
    : serverHost_(host), serverPort_(port), socket_(nullptr), running_(false), currentUser_("")
{
}
Client::~Client()
{
    running_ = false;
    if (socket_)
        socket_->close();
    if (recvThread_.joinable())
        recvThread_.join();
    std::cout << "Клиент завершил работу.\n";
}
void Client::connectToServer()
{
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(60);
    while (std::chrono::steady_clock::now() < deadline)
    {
        socket_ = tcp::createSocket();
        if (!socket_)
        {
            std::cerr << "Ошибка: не удалось создать сокет. Повтор через секунду...\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        std::cout << "Попытка подключения к "
                  << serverHost_ << ":" << serverPort_ << "...\n";
        if (socket_->connect(serverHost_, serverPort_))
        {
            std::cout << "Успешно подключено к серверу.\n";
            running_ = true;
            return;
        }
        else
        {
            std::cerr << "Не удалось подключиться. Повтор через секунду...\n";
            socket_.reset();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    std::cerr << "Таймаут 60 секунд истёк — сервер недоступен. Выходим.\n";
    running_ = false;
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
        std::cout << "Вход отменён.\n";
        return;
    }
    std::string login_packet = "LOGIN " + creds_str + "\n";
    if (socket_->send(login_packet.c_str(), login_packet.length()) != login_packet.length())
    {
        std::cerr << "Ошибка отправки логина. Закрываемся.\n";
        running_ = false;
        return;
    }
    char buffer[TCP_BUFFER_SIZE];
    auto received = socket_->receive(buffer, TCP_BUFFER_SIZE - 1);
    if (received > 0)
    {
        buffer[received] = '\0';
        std::string resp(buffer);
        if (resp.find("LOGIN_OK") != std::string::npos)
        {
            auto space = creds_str.find(' ');
            currentUser_ = (space != std::string::npos
                                ? creds_str.substr(0, space)
                                : creds_str);
            std::cout << "Вход успешен: " << currentUser_ << "\n";
        }
        else
        {
            std::cerr << "Ошибка входа: " << resp;
        }
    }
    else
    {
        std::cerr << "Не получили ответа от сервера. Закрываемся.\n";
        running_ = false;
    }
}
void Client::sendMessage()
{
    if (!socket_ || !running_ || currentUser_.empty())
    {
        std::cerr << "Не подключено или не залогинен.\n";
        return;
    }
    auto msg = UI::promptMessage();
    if (msg.empty())
    {
        std::cout << "Сообщение пустое — не отправляем.\n";
        return;
    }
    std::string packet = "MESSAGE " + currentUser_ + " " + msg + "\n";
    if (socket_->send(packet.c_str(), packet.length()) != packet.length())
    {
        std::cerr << "Ошибка отправки сообщения. Закрываемся.\n";
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
        auto received = socket_->receive(buffer, TCP_BUFFER_SIZE - 1);
        if (received > 0)
        {
            buffer[received] = '\0';
            std::cout << "Получено: " << buffer;
        }
        else if (received == 0)
        {
            std::cout << "Сервер закрыл соединение.\n";
            running_ = false;
        }
        else
        {
            std::cerr << "Ошибка при получении данных.\n";
            running_ = false;
        }
    }
}
void Client::run()
{
    connectToServer();
    if (!running_)
        return;
    recvThread_ = std::thread(&Client::receiveLoop, this);
    while (running_)
    {
        if (currentUser_.empty())
        {
            char cmd = UI::showLoginMenu();
            if (cmd == 'l')
                login();
            else if (cmd == 'q')
                running_ = false;
            else
                std::cout << "Неверный ввод.\n";
        }
        else
        {
            char cmd = UI::showUserMenu(currentUser_);
            if (cmd == 's')
                sendMessage();
            else if (cmd == 'o')
            {
                currentUser_.clear();
                std::cout << "Выход из аккаунта.\n";
            }
            else
                std::cout << "Неверный ввод.\n";
        }
    }
    if (recvThread_.joinable())
        recvThread_.join();
}
