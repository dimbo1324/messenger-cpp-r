#include "Server.h"
#include "../props.h"
#include "../utils/networkMessages.h"

#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>

#if !defined(_WIN32)
#include <fcntl.h>
#endif

namespace Networking
{

    Server::Server(unsigned short port)
        : port_(port), serverSocket_(INVALID_SOCKET_VALUE), running_(false)
    {
    }

    Server::~Server()
    {
        stop();
    }

    bool Server::start()
    {
        serverSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket_ == INVALID_SOCKET_VALUE)
        {
            std::cerr << serverMessages::SERVER_SOCKET_ERROR << std::endl;
            return false;
        }

        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port_);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(serverSocket_, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
        {
            std::cerr << serverMessages::BIND_ERROR << port_ << std::endl;
#if defined(_WIN32)
            closesocket(serverSocket_);
#else
            close(serverSocket_);
#endif
            return false;
        }

        if (listen(serverSocket_, SOMAXCONN) == SOCKET_ERROR)
        {
            std::cerr << serverMessages::LISTEN_ERROR << std::endl;
#if defined(_WIN32)
            closesocket(serverSocket_);
#else
            close(serverSocket_);
#endif
            return false;
        }

        running_ = true;
        std::cout << serverMessages::SERVER_STARTED << port_ << std::endl;

        clientThreads_.emplace_back(&Server::acceptClients, this);
        return true;
    }

    void Server::stop()
    {
        if (!running_)
        {
            return;
        }
        running_ = false;

#if defined(_WIN32)
        closesocket(serverSocket_);
#else
        close(serverSocket_);
#endif
        serverSocket_ = INVALID_SOCKET_VALUE;

        for (std::thread &t : clientThreads_)
        {
            if (t.joinable())
            {
                t.join();
            }
        }
        clientThreads_.clear();

        std::cout << serverMessages::SERVER_STOPPED << std::endl;
    }

    void Server::acceptClients()
    {
        while (running_)
        {
            sockaddr_in clientAddr;
            socklen_t clientAddrSize = sizeof(clientAddr);
            SocketType clientSocket = accept(serverSocket_, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrSize);

            if (clientSocket == INVALID_SOCKET_VALUE)
            {
                if (!running_)
                    break;
                std::cerr << serverMessages::CLIENT_ACCEPT_ERROR << std::endl;
                continue;
            }

            std::cout << serverMessages::NEW_CLIENT_CONNECTED
                      << inet_ntoa(clientAddr.sin_addr) << ":"
                      << ntohs(clientAddr.sin_port) << std::endl;

            clientThreads_.emplace_back(&Server::handleClient, this, clientSocket);
        }
    }

    void Server::handleClient(SocketType clientSocket)
    {
        const int bufferSize = static_cast<int>(BUFFER_SIZE * BUFFER_CELL);
        char buffer[bufferSize];

        while (running_)
        {
            std::memset(buffer, 0, bufferSize);
            int bytesReceived = recv(clientSocket, buffer, bufferSize - 1, 0);
            if (bytesReceived > 0)
            {
                std::cout << "Получено сообщение от клиента: " << buffer << std::endl;

                int bytesSent = send(clientSocket, buffer, bytesReceived, 0);
                if (bytesSent == SOCKET_ERROR)
                {
                    std::cerr << serverMessages::SEND_ERROR << std::endl;
                    break;
                }
            }
            else if (bytesReceived == 0)
            {
                std::cout << serverMessages::CLIENT_DISCONNECTED << std::endl;
                break;
            }
            else
            {
                std::cerr << serverMessages::RECEIVE_ERROR << std::endl;
                break;
            }
        }

#if defined(_WIN32)
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
    }

}
