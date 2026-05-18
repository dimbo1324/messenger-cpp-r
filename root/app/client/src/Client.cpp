#include "Client.h"
#include "UI.h"
#include "tcp/Protocol.h"
#include "tcp/SocketFactory.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

Client::Client(const std::string &host, unsigned short port)
    : serverHost_(host), serverPort_(port)
{
}

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
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(60);
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
    std::cerr << "Таймаут 60 сек - сервер недоступен.\n";
}

bool Client::sendCommand(const std::string &command)
{
    if (!socket_ || !tcp::sendFrame(*socket_, command))
    {
        std::cerr << "Ошибка отправки команды серверу.\n";
        running_ = false;
        return false;
    }
    return true;
}

void Client::registerUser()
{
    const auto creds = UI::promptRegistration();
    if (creds.empty())
    {
        std::cout << "Регистрация отменена.\n";
        return;
    }
    sendCommand("REGISTER " + creds);
}

void Client::login()
{
    const auto creds = UI::promptLogin();
    if (creds.empty())
    {
        std::cout << "Вход отменен.\n";
        return;
    }

    const auto space = creds.find(' ');
    {
        std::lock_guard<std::mutex> lock(userMutex_);
        pendingLogin_ = space == std::string::npos ? creds : creds.substr(0, space);
    }
    sendCommand("LOGIN " + creds);
}

void Client::logout()
{
    sendCommand("LOGOUT");
    clearCurrentUser();
    std::cout << "Вы вышли из аккаунта.\n";
}

void Client::listUsers()
{
    sendCommand("LIST");
}

void Client::inbox()
{
    sendCommand("INBOX");
}

void Client::sendMessage()
{
    if (currentUser().empty())
    {
        std::cerr << "Не залогинен.\n";
        return;
    }

    const auto to = UI::promptTargetUser();
    if (to.empty())
    {
        return;
    }
    const auto msg = UI::promptMessage();
    if (msg.empty())
    {
        return;
    }
    sendCommand("MESSAGE " + to + " " + msg);
}

void Client::history()
{
    if (currentUser().empty())
    {
        std::cerr << "Не залогинен.\n";
        return;
    }
    const auto to = UI::promptTargetUser();
    if (to.empty())
    {
        return;
    }
    sendCommand("HISTORY " + to + " 50 0");
}

std::string Client::currentUser()
{
    std::lock_guard<std::mutex> lock(userMutex_);
    return currentUser_;
}

void Client::setCurrentUser(const std::string &user)
{
    std::lock_guard<std::mutex> lock(userMutex_);
    currentUser_ = user;
    pendingLogin_.clear();
}

void Client::clearCurrentUser()
{
    std::lock_guard<std::mutex> lock(userMutex_);
    currentUser_.clear();
    pendingLogin_.clear();
}

void Client::receiveLoop()
{
    while (running_)
    {
        std::string line;
        const auto status = tcp::receiveFrame(*socket_, line);
        if (status == tcp::ReceiveFrameStatus::Closed)
        {
            running_ = false;
            break;
        }
        if (status != tcp::ReceiveFrameStatus::Ok)
        {
            std::cout << "Соединение закрыто или получен некорректный frame.\n";
            running_ = false;
            break;
        }

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "REGISTER_OK")
        {
            std::cout << "Регистрация OK\n";
        }
        else if (cmd == "REGISTER_ERROR")
        {
            std::string msg;
            std::getline(iss, msg);
            std::cout << "Регистрация FAILED:" << msg << "\n";
        }
        else if (cmd == "LOGIN_OK")
        {
            std::string role;
            iss >> role;
            std::string user;
            {
                std::lock_guard<std::mutex> lock(userMutex_);
                user = pendingLogin_;
            }
            setCurrentUser(user);
            std::cout << "Вход OK, роль: " << role << "\n";
        }
        else if (cmd == "LOGIN_ERROR")
        {
            clearCurrentUser();
            std::string msg;
            std::getline(iss, msg);
            std::cout << "Вход FAILED:" << msg << "\n";
        }
        else if (cmd == "USERS_BEGIN")
        {
            std::cout << "Пользователи:\n";
        }
        else if (cmd == "USER")
        {
            std::string login;
            std::string statusText;
            std::string role;
            std::string lastSeen;
            iss >> login >> statusText >> role >> lastSeen;
            std::cout << " - " << login << " [" << statusText << ", " << role << "]";
            if (!lastSeen.empty())
            {
                std::cout << " last_seen=" << lastSeen;
            }
            std::cout << "\n";
        }
        else if (cmd == "USERS_END")
        {
            std::cout << "-- конец списка --\n";
        }
        else if (cmd == "INBOX_BEGIN")
        {
            std::cout << "Входящие:\n";
        }
        else if (cmd == "INBOX_MSG")
        {
            std::string from;
            std::string createdAt;
            iss >> from >> createdAt;
            std::string msg;
            std::getline(iss, msg);
            std::cout << "[" << createdAt << "] " << from << ":" << msg << "\n";
        }
        else if (cmd == "INBOX_END")
        {
            std::cout << "-- конец входящих --\n";
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
            std::cout << "Сообщение сохранено.\n";
        }
        else if (cmd == "MESSAGE_ERROR")
        {
            std::string msg;
            std::getline(iss, msg);
            std::cout << "Сообщение не отправлено:" << msg << "\n";
        }
        else if (cmd == "HISTORY_BEGIN")
        {
            std::cout << "История:\n";
        }
        else if (cmd == "HIST")
        {
            std::string from;
            std::string to;
            std::string createdAt;
            iss >> from >> to >> createdAt;
            std::string msg;
            std::getline(iss, msg);
            std::cout << "[" << createdAt << "] " << from << " -> " << to << ":" << msg << "\n";
        }
        else if (cmd == "HISTORY_END")
        {
            std::cout << "-- конец истории --\n";
        }
        else if (cmd == "LOGOUT_OK")
        {
            clearCurrentUser();
        }
        else if (cmd == "KICKED")
        {
            clearCurrentUser();
            std::cout << "Сессия завершена администратором.\n";
        }
        else
        {
            std::cout << "< " << line << "\n";
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
        if (currentUser().empty())
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
            switch (UI::showUserMenu(currentUser()))
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
                logout();
                break;
            default:
                std::cout << "Неверный выбор. Попробуйте еще раз.\n";
                break;
            }
        }
    }

    if (socket_)
    {
        socket_->close();
    }
    if (recvThread_.joinable())
    {
        recvThread_.join();
    }
}
