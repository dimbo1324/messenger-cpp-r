#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <mutex>
#include "ClientHandler.h"
#include "serverProps.h"
#include "messages.h"
#include "../ChatServer/ChatServer.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#endif

// Определяем флаг остановки
std::atomic<bool> ClientHandler::stopFlag(false);

void ClientHandler::handleClient(int clientSocket, ChatServer &server)
{
    const size_t initialBufSize = BUF_SIZE;
    std::vector<char> buffer(initialBufSize);

    while (!stopFlag)
    {
        // Читаем данные, если сообщение превышает начальный размер, увеличиваем буфер
        int bytesRead = recv(clientSocket, buffer.data(), buffer.size() - 1, 0);
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            std::cout << SERVER_MESSAGES::MESSAGE_RECEIVED << buffer.data() << std::endl;

            std::string request(buffer.data());
            std::string response = server.handleRequest(request);

            // Отправка данных с обработкой ошибок
            int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
            if (bytesSent == -1)
            {
#ifdef _WIN32
                std::cerr << "Ошибка отправки данных: " << WSAGetLastError() << std::endl;
#else
                std::cerr << "Ошибка отправки данных: " << strerror(errno) << std::endl;
#endif
                break;
            }
        }
        else if (bytesRead == 0)
        {
            std::cout << SERVER_MESSAGES::CLIENT_DISCONNECTED << std::endl;
            break;
        }
        else
        {
#ifdef _WIN32
            std::cerr << "Ошибка при получении сообщения: " << WSAGetLastError() << std::endl;
#else
            std::cerr << "Ошибка при получении сообщения: " << strerror(errno) << std::endl;
#endif
            break;
        }
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}
