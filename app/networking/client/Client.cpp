#include "Client.h"
#include "../props.h"
#include "../utils/messages.h"
#include "Chat.h"
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <memory>
#include <stdexcept>

namespace NetApp
{
    Client::Client(const std::string &serverAddress, unsigned short serverPort, ChatApp::Chat *chat)
        : serverAddress_(serverAddress), serverPort_(serverPort), clientSocket_(INVALID_SOCKET_VALUE),
          connected_(false), receiving_(false), chatPtr_(chat)
    {
    }

    Client::~Client()
    {
        disconnect();
    }

    bool Client::connectToServer()
    {
        clientSocket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket_ == INVALID_SOCKET_VALUE)
        {
            throw std::runtime_error(clientMessages::CLIENT_SOCKET_ERROR);
        }

        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort_);

        if (inet_pton(AF_INET, serverAddress_.c_str(), &serverAddr.sin_addr) <= 0)
        {
#if defined(_WIN32)
            closesocket(clientSocket_);
#else
            close(clientSocket_);
#endif
            throw std::runtime_error(clientMessages::INVALID_ADDRESS);
        }

        if (connect(clientSocket_, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0)
        {
#if defined(_WIN32)
            closesocket(clientSocket_);
#else
            close(clientSocket_);
#endif
            throw std::runtime_error(clientMessages::CONNECT_ERROR);
        }

        connected_ = true;
        std::cout << clientMessages::CONNECT_SUCCESS
                  << serverAddress_ << ":" << serverPort_
                  << " успешно установлено." << std::endl;
        return true;
    }

    void Client::disconnect()
    {
        if (connected_)
        {
            receiving_ = false;
            if (receiveThread_.joinable())
            {
                receiveThread_.join();
            }
#if defined(_WIN32)
            closesocket(clientSocket_);
#else
            close(clientSocket_);
#endif
            clientSocket_ = INVALID_SOCKET_VALUE;
            connected_ = false;
            std::cout << clientMessages::DISCONNECT_SUCCESS << std::endl;
        }
    }

    bool Client::sendMessage(const std::string &message)
    {
        if (!connected_)
            throw std::runtime_error("Нет подключения к серверу");
        int bytesSent = send(clientSocket_, message.c_str(), static_cast<int>(message.size()), 0);
        if (bytesSent == SOCKET_ERROR_VALUE)
        {
            throw std::runtime_error(clientMessages::SEND_ERROR);
        }
        return true;
    }

    void Client::startReceiving()
    {
        if (!connected_)
            return;
        receiving_ = true;
        receiveThread_ = std::thread(&Client::receiveLoop, this);
    }

    void Client::stopReceiving()
    {
        receiving_ = false;
        if (receiveThread_.joinable())
        {
            receiveThread_.join();
        }
    }

    void Client::receiveLoop()
    {
        char buffer[1024];
        while (receiving_ && connected_)
        {
            std::memset(buffer, 0, sizeof(buffer));
            int bytesReceived = ::recv(clientSocket_, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived > 0)
            {
                std::string response(buffer);
                handleServerResponse(response);
            }
            else if (bytesReceived == 0)
            {
                std::cout << "Сервер закрыл соединение\n";
                disconnect();
                return;
            }
            else
            {
                throw std::runtime_error("Ошибка получения данных");
            }
        }
        disconnect();
    }

    bool Client::isConnected() const
    {
        return connected_;
    }

    void Client::handleServerResponse(const std::string &response)
    {
        std::istringstream iss(response);
        std::string command;
        std::getline(iss, command);

        if (command == "LOGIN_OK")
        {
            std::string login;
            std::getline(iss, login);
            if (!login.empty())
            {
                chatPtr_->setCurrentUser(std::make_shared<ChatApp::User>(login, "", ""));
                std::cout << "Вход успешен для " << login << std::endl;
            }
        }
        else if (command == "SIGNUP_OK")
        {
            std::cout << "Регистрация успешна" << std::endl;
        }
        else if (command == "CHAT_MESSAGES")
        {
            std::string message;
            while (std::getline(iss, message))
            {
                if (!message.empty())
                {
                    std::cout << message << std::endl;
                }
            }
        }
        else if (command == "USER_LIST")
        {
            std::string user;
            while (std::getline(iss, user))
            {
                if (!user.empty())
                {
                    std::cout << user << std::endl;
                }
            }
        }
        else if (command == "ERROR")
        {
            std::string errorMsg;
            std::getline(iss, errorMsg);
            if (!errorMsg.empty())
            {
                std::cerr << "Ошибка: " << errorMsg << std::endl;
            }
        }
        else
        {
            std::cout << "Неизвестный ответ от сервера: " << response << std::endl;
        }
    }
}
