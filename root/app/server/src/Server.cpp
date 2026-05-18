#include "Server.h"
#include "ClientHandler.h"
#include "Logger.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <utility>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#endif

Server::Server(ServerConfig config)
    : config_(std::move(config)),
      db_(std::make_shared<Database>(config_.dbConn, config_.dbPoolSize)),
      sessions_(std::make_shared<SessionRegistry>())
{
    Logger::getInstance().configure(config_.logPath);
    if (!db_->ping())
    {
        throw std::runtime_error("database is not reachable");
    }
    initSocket();
}

Server::~Server()
{
    Logger::getInstance().info("server shutting down");
    tcp::closeSocket(serverSocket_);
#ifdef _WIN32
    WSACleanup();
#endif
}

void Server::initSocket()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        throw std::runtime_error("WSAStartup failed");
    }
#endif

    serverSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (!tcp::isValidSocket(serverSocket_))
    {
        throw std::runtime_error("failed to create server socket");
    }

    tcp::setReuseAddress(serverSocket_);
    tcp::setNoSigPipe(serverSocket_);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(config_.port));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (::bind(serverSocket_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        throw std::runtime_error("failed to bind server socket");
    }

    if (::listen(serverSocket_, config_.maxClients) < 0)
    {
        throw std::runtime_error("failed to listen on server socket");
    }

    Logger::getInstance().info("server listening on port " + std::to_string(config_.port));
    if (config_.tlsEnabled)
    {
        Logger::getInstance().warn("TLS is configured but the current transport is still plaintext TCP");
    }
}

void Server::start()
{
    while (true)
    {
        sockaddr_in clientAddr{};
#ifdef _WIN32
        int len = sizeof(clientAddr);
#else
        socklen_t len = sizeof(clientAddr);
#endif
        const tcp::SocketHandle clientSock = ::accept(serverSocket_,
                                                      reinterpret_cast<sockaddr *>(&clientAddr),
                                                      &len);
        if (!tcp::isValidSocket(clientSock))
        {
            Logger::getInstance().warn("failed to accept client");
            continue;
        }

        if (activeClients_.load() >= config_.maxClients)
        {
            tcp::sendFrame(clientSock, "ERROR_SERVER_BUSY");
            tcp::closeSocket(clientSock);
            Logger::getInstance().warn("client rejected: max client limit reached");
            continue;
        }

        tcp::setReceiveTimeout(clientSock, config_.readTimeoutSeconds);
        tcp::setNoSigPipe(clientSock);
        activeClients_.fetch_add(1);

        std::thread([this, clientSock]() {
            try
            {
                ClientHandler handler(clientSock, db_, sessions_, config_.maxFrameSize);
                handler.run();
            }
            catch (const std::exception &e)
            {
                Logger::getInstance().error(std::string("client handler crashed: ") + e.what());
            }
            activeClients_.fetch_sub(1);
        }).detach();
    }
}
