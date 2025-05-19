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
    : serverHost_(host), serverPort_(port), socket_(nullptr), running_(false) {}

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
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        std::cout << "Попытка подключения к " << serverHost_ << ":" << serverPort_ << "...\n";
        if (socket_->connect(serverHost_, serverPort_))
        {
            std::cout << "Успешно подключено к серверу.\n";
            running_ = true;
            return;
        }
        socket_.reset();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cerr << "Таймаут 60 сек — сервер недоступен.\n";
}

void Client::registerUser()
{
    auto creds = UI::promptRegistration();
    if (creds.empty())
    {
        std::cout << "Регистрация отменена.\n";
        return;
    }

    std::string packet = "REGISTER " + creds + "\n";
    socket_->send(packet.c_str(), packet.size());
    char buf[TCP_BUFFER_SIZE];
    auto r = socket_->receive(buf, TCP_BUFFER_SIZE - 1);
    if (r <= 0)
    {
        std::cerr << "Нет ответа сервера.\n";
        return;
    }
    buf[r] = '\0';
    std::string resp(buf);
    if (resp.find("REGISTER_OK") != std::string::npos)
        std::cout << "Регистрация успешна.\n";
    else
        std::cout << "Ошибка регистрации: " << resp;
}

void Client::login()
{
    auto creds = UI::promptLogin();
    if (creds.empty())
    {
        std::cout << "Вход отменён.\n";
        return;
    }
    std::string packet = "LOGIN " + creds + "\n";
    socket_->send(packet.c_str(), packet.size());
    char buf[TCP_BUFFER_SIZE];
    auto r = socket_->receive(buf, TCP_BUFFER_SIZE - 1);
    if (r <= 0)
    {
        std::cerr << "Нет ответа сервера.\n";
        return;
    }
    buf[r] = '\0';
    std::string resp(buf);
    if (resp.find("LOGIN_OK") != std::string::npos)
    {
        currentUser_ = creds.substr(0, creds.find(' '));
        std::cout << "Вход успешен: " << currentUser_ << "\n";
    }
    else
    {
        std::cout << "Ошибка входа: " << resp;
    }
}

void Client::listUsers()
{
    std::string packet = "LIST\n";
    socket_->send(packet.c_str(), packet.size());
    char buf[TCP_BUFFER_SIZE];
    auto r = socket_->receive(buf, TCP_BUFFER_SIZE - 1);
    if (r > 0)
    {
        buf[r] = '\0';
        std::cout << "Online: " << buf;
    }
}

void Client::sendMessage()
{
    if (currentUser_.empty())
    {
        std::cerr << "Не залогинен.\n";
        return;
    }
    auto target = UI::promptTargetUser();
    if (target.empty())
        return;
    auto msg = UI::promptMessage();
    if (msg.empty())
        return;
    std::string packet = "MESSAGE " + target + " " + msg + "\n";
    socket_->send(packet.c_str(), packet.size());
    char buf[TCP_BUFFER_SIZE];
    auto r = socket_->receive(buf, TCP_BUFFER_SIZE - 1);
    if (r > 0)
    {
        buf[r] = '\0';
        std::cout << buf;
    }
}

void Client::history()
{
    if (currentUser_.empty())
        return;
    auto target = UI::promptTargetUser();
    if (target.empty())
        return;
    std::string packet = "HISTORY " + target + "\n";
    socket_->send(packet.c_str(), packet.size());
    char buf[TCP_BUFFER_SIZE];
    while (true)
    {
        auto r = socket_->receive(buf, TCP_BUFFER_SIZE - 1);
        if (r <= 0)
            break;
        buf[r] = '\0';
        std::string line(buf);
        if (line.find("HISTORY_END") != std::string::npos)
            break;
        std::cout << line;
    }
}

void Client::receiveLoop()
{
    while (running_)
    {
        char buf[TCP_BUFFER_SIZE];
        auto r = socket_->receive(buf, TCP_BUFFER_SIZE - 1);
        if (r > 0)
        {
            buf[r] = '\0';
            std::cout << "<" << buf;
        }
        else
        {
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
            char cmd = UI::showInitialMenu();
            if (cmd == 'l')
                login();
            else if (cmd == 'r')
                registerUser();
            else if (cmd == 'q')
                running_ = false;
        }
        else
        {
            char cmd = UI::showUserMenu(currentUser_);
            if (cmd == 'l')
                listUsers();
            else if (cmd == 's')
                sendMessage();
            else if (cmd == 'h')
                history();
            else if (cmd == 'o')
            {
                currentUser_.clear();
            }
        }
    }

    if (recvThread_.joinable())
        recvThread_.join();
}
