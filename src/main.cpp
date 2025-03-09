#include <thread>
#include <iostream>
#include "network/server/messages.h"
#include "network/server/serverProps.h"
#include "network/server/ClientHandler/ClientHandler.h"
#include "network/server/ServerConnection/ServerConnection.h"
#include "network/client/ChatClient/ChatClient.h"
#include "network/server/ChatServer/ChatServer.h"

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << SERVER_MESSAGES::WINSOCK_INIT_ERROR << WSAGetLastError() << std::endl;
        return 1;
    }
#endif

    ServerConnection server;
    if (!server.initialize())
    {
        return 1;
    }

    std::cout << SERVER_MESSAGES::SERVER_STARTED << PORT << std::endl;

    ChatServer chatServer;

    while (true)
    {
        int clientSocket = server.acceptClient();
        if (clientSocket != -1)
        {
            std::thread clientThread(ClientHandler::handleClient, clientSocket, std::ref(chatServer));
            clientThread.detach();
        }
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}