#include <string>
#include <iostream>
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

void ClientHandler::handleClient(int clientSocket, ChatServer &server)
{
    char buffer[BUF_SIZE];
    while (true)
    {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            std::cout << SERVER_MESSAGES::MESSAGE_RECEIVED << buffer << std::endl;
            std::string request(buffer);
            std::string response = server.handleRequest(request);
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if (bytesRead == 0)
        {
            std::cout << SERVER_MESSAGES::CLIENT_DISCONNECTED << std::endl;
            break;
        }
        else
        {
            std::cerr << SERVER_MESSAGES::RECV_ERROR << std::endl;
            break;
        }
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}