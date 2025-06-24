#include "Server.h"
#include "ClientHandler.h"
#include "Logger.h"
#include "threading/ThreadPool.h"
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif
#include <cstring>
#include <cstdlib>
#include <iostream>
Server::Server(int port)
    : port(port), serverSocket(-1)
{
    initSocket();
    db_ = std::make_shared<Database>("dbname=chat_db user=postgres password=545687 host=localhost port=5432");
}
Server::~Server()
{
    Logger::getInstance().log("Server shutting down");
#ifdef _WIN32
    closesocket(serverSocket);
    WSACleanup();
#else
    close(serverSocket);
#endif
}
void Server::initSocket()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed\n";
        std::exit(EXIT_FAILURE);
    }
#endif
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("socket");
        std::exit(EXIT_FAILURE);
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));
    addr.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
    if (bind(serverSocket, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        std::exit(EXIT_FAILURE);
    }
    if (listen(serverSocket, 10) < 0)
    {
        perror("listen");
        std::exit(EXIT_FAILURE);
    }
    Logger::getInstance().log("Server listening on port " + std::to_string(port));
}
void Server::start()
{
    threading::ThreadPool pool(4);
    while (true)
    {
        sockaddr_in clientAddr{};
        socklen_t len = sizeof(clientAddr);
        int clientSock = accept(serverSocket, (sockaddr *)&clientAddr, &len);
        if (clientSock < 0)
        {
            Logger::getInstance().log("Failed to accept client");
            continue;
        }
        pool.enqueue([clientSock, db = db_]()
                     {
ClientHandler handler(clientSock, db);
handler.run(); });
    }
}