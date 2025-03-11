#include "NetApp.h"
#include <iostream>
#include <cstring>
#include <sstream>

#if defined(_WIN32)
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

namespace Networking
{

#if defined(_WIN32)
    struct WinsockInitializer
    {
        WinsockInitializer()
        {
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                std::cerr << "WSAStartup failed\n";
                exit(1);
            }
        }
        ~WinsockInitializer()
        {
            WSACleanup();
        }
    } winsockInit;
#endif

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
        serverSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket_ == INVALID_SOCKET_VALUE)
        {
            std::cerr << "Ошибка создания сокета\n";
            return false;
        }

        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port_);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (::bind(serverSocket_, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR_VALUE)
        {
            std::cerr << "Ошибка привязки сокета\n";
#if defined(_WIN32)
            ::closesocket(serverSocket_);
#else
            ::close(serverSocket_);
#endif
            return false;
        }

        if (::listen(serverSocket_, SOMAXCONN) == SOCKET_ERROR_VALUE)
        {
            std::cerr << "Ошибка прослушивания сокета\n";
#if defined(_WIN32)
            ::closesocket(serverSocket_);
#else
            ::close(serverSocket_);
#endif
            return false;
        }

        running_ = true;
        std::cout << "Сервер запущен на порту " << port_ << "\n";
        clientThreads_.emplace_back(&Server::acceptClients, this);
        return true;
    }

    void Server::stop()
    {
        if (!running_)
            return;
        running_ = false;
#if defined(_WIN32)
        ::closesocket(serverSocket_);
#else
        ::close(serverSocket_);
#endif
        for (auto &t : clientThreads_)
        {
            if (t.joinable())
                t.join();
        }
        clientThreads_.clear();
        std::cout << "Сервер остановлен\n";
    }

    void Server::acceptClients()
    {
        while (running_)
        {
            sockaddr_in clientAddr;
            socklen_t clientAddrSize = sizeof(clientAddr);
            SocketType clientSocket = ::accept(serverSocket_, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrSize);
            if (clientSocket == INVALID_SOCKET_VALUE)
            {
                if (!running_)
                    break;
                std::cerr << "Ошибка приема клиента\n";
                continue;
            }
            std::cout << "Новый клиент подключился\n";
            clientThreads_.emplace_back(&Server::handleClient, this, clientSocket);
        }
    }

    void Server::handleClient(SocketType clientSocket)
    {
        char buffer[1024];
        while (running_)
        {
            std::memset(buffer, 0, sizeof(buffer));
            int bytesReceived = ::recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived <= 0)
            {
                handleDisconnect(clientSocket);
                break;
            }
            std::string request(buffer);
            std::string response = processRequest(clientSocket, request);
            ::send(clientSocket, response.c_str(), response.size(), 0);
        }
#if defined(_WIN32)
        ::closesocket(clientSocket);
#else
        ::close(clientSocket);
#endif
    }

    std::string Server::processRequest(SocketType clientSocket, const std::string &request)
    {
        std::istringstream iss(request);
        std::string command;
        iss >> command;
        if (command == "LOGIN")
        {
            return "OK Login successful";
        }
        else if (command == "SIGNUP")
        {
            return "OK Signup successful";
        }
        return "ERROR Unknown command";
    }

    void Server::handleDisconnect(SocketType clientSocket)
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        clientToUser_.erase(clientSocket);
    }

    void Server::broadcastMessage(const std::string &message)
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        for (const auto &[socket, user] : clientToUser_)
        {
            ::send(socket, message.c_str(), message.size(), 0);
        }
    }

    Client::Client(const std::string &serverAddress, unsigned short serverPort)
        : serverAddress_(serverAddress), serverPort_(serverPort), clientSocket_(INVALID_SOCKET_VALUE), connected_(false), receiving_(false)
    {
    }

    Client::~Client()
    {
        disconnect();
    }

    bool Client::connectToServer()
    {
        clientSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket_ == INVALID_SOCKET_VALUE)
        {
            std::cerr << "Ошибка создания сокета\n";
            return false;
        }

        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort_);
        if (inet_pton(AF_INET, serverAddress_.c_str(), &serverAddr.sin_addr) <= 0)
        {
            std::cerr << "Некорректный адрес сервера\n";
#if defined(_WIN32)
            ::closesocket(clientSocket_);
#else
            ::close(clientSocket_);
#endif
            return false;
        }

        if (::connect(clientSocket_, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0)
        {
            std::cerr << "Ошибка подключения к серверу\n";
#if defined(_WIN32)
            ::closesocket(clientSocket_);
#else
            ::close(clientSocket_);
#endif
            return false;
        }

        connected_ = true;
        std::cout << "Подключено к серверу\n";
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
            ::closesocket(clientSocket_);
#else
            ::close(clientSocket_);
#endif
            connected_ = false;
            std::cout << "Отключено от сервера\n";
        }
    }

    bool Client::sendMessage(const std::string &message)
    {
        if (!connected_)
            return false;
        int bytesSent = ::send(clientSocket_, message.c_str(), message.size(), 0);
        if (bytesSent == SOCKET_ERROR_VALUE)
        {
            std::cerr << "Ошибка отправки сообщения\n";
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
        char buffer[1024];
        while (receiving_ && connected_)
        {
            std::memset(buffer, 0, sizeof(buffer));
            int bytesReceived = ::recv(clientSocket_, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived > 0)
            {
                std::string response(buffer);
                std::cout << "Получено: " << response << "\n";
            }
            else if (bytesReceived == 0)
            {
                std::cout << "Сервер закрыл соединение\n";
                break;
            }
            else
            {
                std::cerr << "Ошибка получения данных\n";
                break;
            }
        }
        disconnect();
    }

    bool Client::isConnected() const
    {
        return connected_;
    }

}
