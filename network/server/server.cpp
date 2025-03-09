#include <iostream>
#include "serverProps/constants.h"
#include "Network/utils/messages.h"

#ifdef _WIN32
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << SERVER_MESSAGES::WINSOCK_INIT_ERROR
#ifdef _WIN32
                  << WSAGetLastError()
#else
                  << errno
#endif
                  << std::endl;
        return 1;
    }
#endif

    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == -1)
    {
#ifdef _WIN32
        std::cerr << SERVER_MESSAGES::SOCKET_CREATE_ERROR << WSAGetLastError() << std::endl;
        WSACleanup();
#else
        std::cerr << SERVER_MESSAGES::SOCKET_CREATE_ERROR << errno << std::endl;
#endif
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == -1)
    {
#ifdef _WIN32
        std::cerr << SERVER_MESSAGES::BIND_ERROR << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
#else
        std::cerr << SERVER_MESSAGES::BIND_ERROR << errno << std::endl;
        close(serverSocket);
#endif
        return 1;
    }

    if (listen(serverSocket, MAX_CLIENTS) == -1)
    {
#ifdef _WIN32
        std::cerr << SERVER_MESSAGES::LISTEN_ERROR << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
#else
        std::cerr << SERVER_MESSAGES::LISTEN_ERROR << errno << std::endl;
        close(serverSocket);
#endif
        return 1;
    }
    else
    {
        std::cout << SERVER_MESSAGES::SERVER_STARTED << PORT << std::endl;
    }

    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressLength);
    if (clientSocket == -1)
    {
#ifdef _WIN32
        std::cerr << SERVER_MESSAGES::ACCEPT_ERROR << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
#else
        std::cerr << SERVER_MESSAGES::ACCEPT_ERROR << errno << std::endl;
        close(serverSocket);
#endif
        return 1;
    }
    else
    {
        std::cout << SERVER_MESSAGES::CLIENT_CONNECTED << inet_ntoa(clientAddress.sin_addr) << std::endl;
    }

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

#ifdef _WIN32
    closesocket(serverSocket);
    WSACleanup();
#else
    close(serverSocket);
#endif

    return 0;
}