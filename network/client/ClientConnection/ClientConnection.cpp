#include <iostream>
#include <thread>
#include "ClientConnection.h"

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace ChatApp
{
    ClientConnection::ClientConnection(const std::string &ip, int port) : _socket(-1)
    {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "Ошибка инициализации Winsock\n";
            return;
        }
#endif
        _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_socket == -1)
        {
            std::cerr << "Ошибка создания сокета\n";
            return;
        }

        _serverAddress.sin_family = AF_INET;
        _serverAddress.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &_serverAddress.sin_addr);

        if (connect(_socket, (sockaddr *)&_serverAddress, sizeof(_serverAddress)) == -1)
        {
            std::cerr << "Ошибка подключения к серверу\n";
#ifdef _WIN32
            closesocket(_socket);
#else
            close(_socket);
#endif
            _socket = -1;
        }
        else
        {
            std::thread(&ClientConnection::receiveMessages, this).detach();
        }
    }

    ClientConnection::~ClientConnection()
    {
        if (_socket != -1)
        {
#ifdef _WIN32
            closesocket(_socket);
            WSACleanup();
#else
            close(_socket);
#endif
        }
    }

    std::string ClientConnection::sendRequest(const std::string &request)
    {
        if (_socket == -1)
            return "ERROR:Нет подключения";
        send(_socket, request.c_str(), request.length(), 0);
        char buffer[1024];
        int bytesReceived = recv(_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0';
            return std::string(buffer);
        }
        return "ERROR:Нет ответа от сервера";
    }

    void ClientConnection::receiveMessages()
    {
        char buffer[1024];
        while (true)
        {
            int bytesReceived = recv(_socket, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived > 0)
            {
                buffer[bytesReceived] = '\0';
                std::cout << "Новое сообщение: " << buffer << std::endl;
            }
            else if (bytesReceived == 0)
            {
                std::cout << "Сервер отключился.\n";
                break;
            }
            else
            {
                std::cerr << "Ошибка при получении сообщения.\n";
                break;
            }
        }
    }
}