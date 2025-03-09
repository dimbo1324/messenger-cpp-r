#include "ClientConnection.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <errno.h>
#endif

namespace ChatApp
{
    ClientConnection::ClientConnection(const std::string &ip, int port) : _socket(-1), _stop(false)
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
        if (inet_pton(AF_INET, ip.c_str(), &_serverAddress.sin_addr) <= 0)
        {
            std::cerr << "Ошибка преобразования IP-адреса\n";
#ifdef _WIN32
            closesocket(_socket);
#else
            close(_socket);
#endif
            _socket = -1;
            return;
        }

        if (connect(_socket, reinterpret_cast<sockaddr *>(&_serverAddress), sizeof(_serverAddress)) == -1)
        {
            std::cerr << "Ошибка подключения к серверу: " << std::strerror(errno) << "\n";
#ifdef _WIN32
            closesocket(_socket);
            WSACleanup();
#else
            close(_socket);
#endif
            _socket = -1;
            return;
        }
        _receiveThread = std::thread(&ClientConnection::receiveMessages, this);
    }

    ClientConnection::~ClientConnection()
    {
        _stop = true;
        {
            std::lock_guard<std::mutex> lock(_socketMutex);
            if (_socket != -1)
            {
#ifdef _WIN32
                shutdown(_socket, SD_BOTH);
                closesocket(_socket);
#else
                shutdown(_socket, SHUT_RDWR);
                close(_socket);
#endif
                _socket = -1;
            }
        }
        if (_receiveThread.joinable())
        {
            _receiveThread.join();
        }
#ifdef _WIN32
        WSACleanup();
#endif
    }

    std::string ClientConnection::sendRequest(const std::string &request)
    {
        if (_socket == -1)
            return "ERROR:Нет подключения";

        std::lock_guard<std::mutex> lock(_socketMutex);
        int bytesSent = send(_socket, request.c_str(), request.length(), 0);
        if (bytesSent == -1)
        {
            std::ostringstream err;
            err << "ERROR:Ошибка при отправке данных: " << std::strerror(errno);
            return err.str();
        }

        std::vector<char> buffer(4096);
        int bytesReceived = recv(_socket, buffer.data(), buffer.size() - 1, 0);
        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0';
            return std::string(buffer.data());
        }
        else if (bytesReceived == 0)
        {
            return "ERROR:Соединение закрыто сервером";
        }
        else
        {
            std::ostringstream err;
            err << "ERROR:Ошибка при получении ответа: " << std::strerror(errno);
            return err.str();
        }
    }

    void ClientConnection::receiveMessages()
    {
        std::vector<char> buffer(4096);
        while (!_stop)
        {
            int bytesReceived = -1;
            {
                std::lock_guard<std::mutex> lock(_socketMutex);
                if (_socket != -1)
                {
                    bytesReceived = recv(_socket, buffer.data(), buffer.size() - 1, 0);
                }
            }
            if (bytesReceived > 0)
            {
                buffer[bytesReceived] = '\0';
                std::cout << "Новое сообщение: " << buffer.data() << std::endl;
            }
            else if (bytesReceived == 0)
            {
                std::cout << "Сервер отключился.\n";
                break;
            }
            else
            {
                if (!_stop)
                {
                    std::cerr << "Ошибка при получении сообщения: " << std::strerror(errno) << "\n";
                }
                break;
            }
        }
    }
}
