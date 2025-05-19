#include "Client.h"
#include "UI.h"
#include "tcp/SocketFactory.h"
#include "tcp_config.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
Client::Client(const std::string &host, unsigned short port)
    : serverHost_(host),
      serverPort_(port),
      socket_(nullptr),
      running_(false)
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
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        std::cout << "Попытка подключения к " << serverHost_
                  << ":" << serverPort_ << "...\n";
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
}
void Client::login()
{
    auto creds = UI::promptLogin();
    if (creds.empty())
    {
        std::cout << "Вход отменён.\n";
        return;
    }
    auto spacePos = creds.find(' ');
    std::string loginName = (spacePos != std::string::npos
                                 ? creds.substr(0, spacePos)
                                 : creds);
    currentUser_ = loginName;
    std::string packet = "LOGIN " + creds + "\n";
    socket_->send(packet.c_str(), packet.size());
}
void Client::listUsers()
{
    std::string packet = "LIST\n";
    socket_->send(packet.c_str(), packet.size());
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
    {
        std::cout << "Пустое сообщение не отправляется.\n";
        return;
    }
    std::string packet = "MESSAGE " + target + " " + msg + "\n";
    socket_->send(packet.c_str(), packet.size());
}
void Client::history()
{
    if (currentUser_.empty())
    {
        std::cerr << "Не залогинен.\n";
        return;
    }
    auto target = UI::promptTargetUser();
    if (target.empty())
        return;
    std::string packet = "HISTORY " + target + "\n";
    socket_->send(packet.c_str(), packet.size());
}
void Client::receiveLoop()
{
    while (running_)
    {
        char buf[TCP_BUFFER_SIZE];
        auto r = socket_->receive(buf, TCP_BUFFER_SIZE - 1);
        if (r <= 0)
        {
            running_ = false;
            break;
        }
        buf[r] = '\0';
        std::string line(buf);
        if (!line.empty() && line.back() == '\n')
            line.pop_back();
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd == "REGISTER_OK")
        {
            std::cout << "Регистрация прошла успешно!\n";
        }
        else if (cmd == "REGISTER_ERROR")
        {
            std::cout << "Ошибка регистрации.\n";
        }
        else if (cmd == "LOGIN_OK")
        {
            std::cout << "Вход выполнен успешно!\n";
        }
        else if (cmd == "LOGIN_ERROR")
        {
            std::cout << "Неверный логин или пароль.\n";
            currentUser_.clear();
        }
        else if (cmd == "USERS")
        {
            std::string user;
            std::cout << "Сейчас в чате: ";
            while (iss >> user)
                std::cout << user << " ";
            std::cout << "\n";
        }
        else if (cmd == "MESSAGE")
        {
            std::string from;
            iss >> from;
            std::string msg;
            std::getline(iss, msg);
            std::cout << "[" << from << "]:" << msg << "\n";
        }
        else if (cmd == "HIST")
        {
            std::string msg;
            std::getline(iss, msg);
            std::cout << msg << "\n";
        }
        else if (cmd == "HISTORY_END")
        {
            std::cout << "— конец истории —\n";
        }
        else
        {
            std::cout << "<" << line << "\n";
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
                currentUser_.clear();
        }
    }
    if (recvThread_.joinable())
        recvThread_.join();
}
