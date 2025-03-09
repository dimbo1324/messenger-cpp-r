#include <string>
#include <iostream>

#include "ClientHandler.h"
#include "serverProps/constants.h"
#include "Network/utils/messages.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#endif

void ClientHandler::handleClient(int clientSocket)
{
    char buffer[BUF_SIZE];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead > 0)
    {
        buffer[bytesRead] = '\0';
        std::cout << SERVER_MESSAGES::MESSAGE_RECEIVED << buffer << std::endl;
    }
    else if (bytesRead == 0)
    {
        std::cout << SERVER_MESSAGES::CLIENT_DISCONNECTED << std::endl;
    }
    else
    {
        std::cerr << SERVER_MESSAGES::RECV_ERROR << std::endl;
    }

    std::string response = "Привет, клиент!";
    int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
    if (bytesSent == -1)
    {
#ifdef _WIN32
        std::cerr << SERVER_MESSAGES::SEND_ERROR << WSAGetLastError() << std::endl;
#else
        std::cerr << SERVER_MESSAGES::SEND_ERROR << errno << std::endl;
#endif
    }
    else
    {
        std::cout << SERVER_MESSAGES::RESPONSE_SENT << std::endl;
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}