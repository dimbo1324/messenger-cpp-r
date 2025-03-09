#include <iostream>
#include "ServerConnection.h"
#include "serverProps.h"
#include "messages.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#endif

ServerConnection::ServerConnection() : serverSocket(-1)
{
#ifdef _WIN32
    WSADATA wsaData;
    int wsaInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaInit != 0)
    {
        std::cerr << "Ошибка инициализации Winsock: " << wsaInit << std::endl;
    }
#endif
}

ServerConnection::~ServerConnection()
{
#ifdef _WIN32
    if (serverSocket != -1)
    {
        closesocket(serverSocket);
    }
    WSACleanup();
#else
    if (serverSocket != -1)
    {
        close(serverSocket);
    }
#endif
}

bool ServerConnection::initialize()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == -1)
    {
#ifdef _WIN32
        std::cerr << SERVER_MESSAGES::SOCKET_CREATE_ERROR << WSAGetLastError() << std::endl;
#else
        std::cerr << SERVER_MESSAGES::SOCKET_CREATE_ERROR << errno << std::endl;
#endif
        return false;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == -1)
    {
#ifdef _WIN32
        std::cerr << SERVER_MESSAGES::BIND_ERROR << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
#else
        std::cerr << SERVER_MESSAGES::BIND_ERROR << errno << std::endl;
        close(serverSocket);
#endif
        return false;
    }

    if (listen(serverSocket, MAX_CLIENTS) == -1)
    {
#ifdef _WIN32
        std::cerr << SERVER_MESSAGES::LISTEN_ERROR << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
#else
        std::cerr << SERVER_MESSAGES::LISTEN_ERROR << errno << std::endl;
        close(serverSocket);
#endif
        return false;
    }

    return true;
}

int ServerConnection::acceptClient()
{
    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressLength);
    if (clientSocket == -1)
    {
#ifdef _WIN32
        std::cerr << SERVER_MESSAGES::ACCEPT_ERROR << WSAGetLastError() << std::endl;
#else
        std::cerr << SERVER_MESSAGES::ACCEPT_ERROR << errno << std::endl;
#endif
        return -1;
    }
    else
    {
        std::cout << SERVER_MESSAGES::CLIENT_CONNECTED << inet_ntoa(clientAddress.sin_addr) << std::endl;
    }
    return clientSocket;
}
