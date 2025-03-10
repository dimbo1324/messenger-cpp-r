#include "Client.h"
#include "../props.h"
#include "../utils/networkMessages.h"

#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>

namespace Networking
{

    Client::Client(const std::string &serverAddress, unsigned short serverPort)
        : serverAddress_(serverAddress), serverPort_(serverPort),
          clientSocket_(INVALID_SOCKET_VALUE), connected_(false), receiving_(false)
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
            std::cerr << clientMessages::CLIENT_SOCKET_ERROR << std::endl;
            return false;
        }

        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort_);

        if (inet_pton(AF_INET, serverAddress_.c_str(), &serverAddr.sin_addr) <= 0)
        {
            std::cerr << clientMessages::INVALID_ADDRESS << std::endl;
#if defined(_WIN32)
            closesocket(clientSocket_);
#else
            close(clientSocket_);
#endif
            return false;
        }

        if (connect(clientSocket_, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0)
        {
            std::cerr << clientMessages::CONNECT_ERROR << std::endl;
#if defined(_WIN32)
            closesocket(clientSocket_);
#else
            close(clientSocket_);
#endif
            return false;
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
            return false;
        int bytesSent = send(clientSocket_, message.c_str(), static_cast<int>(message.size()), 0);
        if (bytesSent == SOCKET_ERROR)
        {
            std::cerr << clientMessages::SEND_ERROR << std::endl;
            return false;
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
        const int bufferSize = static_cast<int>(BUFFER_SIZE * BUFFER_CELL);
        char buffer[bufferSize];
        while (receiving_ && connected_)
        {
            std::memset(buffer, 0, bufferSize);
            int bytesReceived = recv(clientSocket_, buffer, bufferSize - 1, 0);
            if (bytesReceived > 0)
            {
                std::cout << "Получено сообщение: " << buffer << std::endl;
            }
            else if (bytesReceived == 0)
            {
                std::cout << clientMessages::SERVER_CLOSED << std::endl;
                break;
            }
            else
            {
                std::cerr << clientMessages::RECEIVE_ERROR << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        disconnect();
    }

    bool Client::isConnected() const
    {
        return connected_;
    }

}
