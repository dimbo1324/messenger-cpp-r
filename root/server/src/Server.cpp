#include "Server.h"
#include "ClientHandler.h"
#include "Logger.h"
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif
namespace server
{
    Server::Server(const std::string &host, unsigned short port)
        : host_(host), port_(port), listenFd_(-1) {}
    Server::~Server()
    {
        Logger::instance().log("Server", "Остановка сервера");
#ifdef _WIN32
        closesocket(listenFd_);
        WSACleanup();
#else
        ::close(listenFd_);
#endif
    }
    void Server::setupSocket()
    {
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
        listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listenFd_ < 0)
        {
            throw std::runtime_error("Не удалось создать сокет");
        }
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        addr.sin_addr.s_addr = inet_addr(host_.c_str());
        int opt = 1;
        setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR,
                   reinterpret_cast<char *>(&opt), sizeof(opt));
        if (bind(listenFd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
        {
            throw std::runtime_error("Bind error");
        }
        if (listen(listenFd_, 5) < 0)
        {
            throw std::runtime_error("Listen error");
        }
        Logger::instance().log("Server", "Слушаем " + host_ + ":" + std::to_string(port_));
    }
    void Server::acceptLoop()
    {
        while (true)
        {
            sockaddr_in clientAddr;
            socklen_t len = sizeof(clientAddr);
            int clientFd = accept(listenFd_,
                                  reinterpret_cast<sockaddr *>(&clientAddr),
                                  &len);
            if (clientFd < 0)
            {
                Logger::instance().log("Error", "Accept failed");
                continue;
            }
            auto handler = std::make_unique<ClientHandler>(clientFd);
            handler->start();
            clients_.push_back(std::move(handler));
        }
    }
    void Server::run()
    {
        setupSocket();
        acceptLoop();
    }
}
