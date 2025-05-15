#include "ClientHandler.h"
#include "Logger.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <cstring>

ClientHandler::ClientHandler(int socket)
    : clientSocket(socket),
      handlerThread(&ClientHandler::run, this)
{
    handlerThread.detach();
}

ClientHandler::~ClientHandler()
{
}

void ClientHandler::run()
{
    char buffer[1024];
    Logger::getInstance().log("Client connected: socket=" + std::to_string(clientSocket));

    while (true)
    {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytes <= 0)
        {
            Logger::getInstance().log("Client disconnected: socket=" + std::to_string(clientSocket));
            break;
        }

        std::string msg(buffer, bytes);
        Logger::getInstance().log("Received from " + std::to_string(clientSocket) + ": " + msg);

        send(clientSocket, buffer, bytes, 0);
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}
