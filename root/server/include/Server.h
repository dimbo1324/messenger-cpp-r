#pragma once
#include <string>
#include <vector>
#include <memory>
namespace server
{
    class ClientHandler;
    class Server
    {
    public:
        Server(const std::string &host, unsigned short port);
        ~Server();
        void run();

    private:
        std::string host_;
        unsigned short port_;
        int listenFd_;
        std::vector<std::unique_ptr<ClientHandler>> clients_;
        void setupSocket();
        void acceptLoop();
    };
}
