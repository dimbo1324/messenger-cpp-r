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
    socket_->send(("REGISTER " + creds + "\n").c_str(), creds.size() + 10);
}
void Client::login()
{
    auto creds = UI::promptLogin();
    if (creds.empty())
    {
        std::cout << "Вход отменён.\n";
        return;
    }
    auto space = creds.find(' ');
    currentUser_ = creds.substr(0, space);
    socket_->send(("LOGIN " + creds + "\n").c_str(), creds.size() + 6);
}
void Client::listUsers()
{
    socket_->send("LIST\n", 5);
}
void Client::inbox()
{
    socket_->send("INBOX\n", 6);
}
void Client::sendMessage()
{
    if (currentUser_.empty())
    {
        std::cerr << "Не залогинен.\n";
        return;
    }
    auto to = UI::promptTargetUser();
    if (to.empty())
        return;
    auto msg = UI::promptMessage();
    if (msg.empty())
        return;
    socket_->send(("MESSAGE " + to + " " + msg + "\n").c_str(),
                  9 + to.size() + msg.size());
}
void Client::history()
{
    if (currentUser_.empty())
    {
        std::cerr << "Не залогинен.\n";
        return;
    }
    auto to = UI::promptTargetUser();
    if (to.empty())
        return;
    socket_->send(("HISTORY " + to + "\n").c_str(), 9 + to.size());
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
            std::cout << "Регистрация OK\n";
        else if (cmd == "REGISTER_ERROR")
            std::cout << "Регистрация FAILED\n";
        else if (cmd == "LOGIN_OK")
            std::cout << "Вход OK\n";
        else if (cmd == "LOGIN_ERROR")
        {
            std::cout << "Вход FAILED\n";
            currentUser_.clear();
        }
        else if (cmd == "USERS")
        {
            std::cout << "Онлайн: ";
            std::string u;
            while (iss >> u)
                std::cout << u << " ";
            std::cout << "\n";
        }
        else if (cmd == "INBOX_COUNT")
        {
            int cnt;
            iss >> cnt;
            std::cout << "У вас " << cnt << " новых письма(ь).\n";
        }
        else if (cmd == "INBOX_MSG")
        {
            std::string from;
            iss >> from;
            std::string msg;
            std::getline(iss, msg);
            std::cout << "[" << from << "]:" << msg << "\n";
        }
        else if (cmd == "INBOX_END")
        {
            std::cout << "— конец входящих —\n";
        }
        else if (cmd == "MESSAGE")
        {
            std::string from;
            iss >> from;
            std::string msg;
            std::getline(iss, msg);
            std::cout << "[" << from << "]:" << msg << "\n";
        }
        else if (cmd == "MESSAGE_OK")
        {
            std::cout << "Сообщение отправлено.\n";
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
            switch (UI::showInitialMenu())
            {
            case 'l':
                login();
                break;
            case 'r':
                registerUser();
                break;
            case 'q':
                running_ = false;
                break;
            default:
                std::cout << "Неверный выбор. Попробуйте еще раз.\n";
                break;
            }
        }
        else
        {
            switch (UI::showUserMenu(currentUser_))
            {
            case 'l':
                listUsers();
                break;
            case 'i':
                inbox();
                break;
            case 's':
                sendMessage();
                break;
            case 'h':
                history();
                break;
            case 'o':
                currentUser_.clear();
                std::cout << "Вы вышли из аккаунта.\n";
                break;
            default:
                std::cout << "Неверный выбор. Попробуйте еще раз.\n";
                break;
            }
        }
    }
    if (recvThread_.joinable())
        recvThread_.join();
}