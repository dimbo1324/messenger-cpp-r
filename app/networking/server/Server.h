#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include "SocketHandler.h"
#include "NetworkInitializer.h"

namespace Networking
{

    class Server
    {
    public:
        explicit Server(unsigned short port);
        ~Server();

        bool start();
        void stop();

        Server(const Server &) = delete;
        Server &operator=(const Server &) = delete;

    private:
        void acceptClients();
        void handleClient(SocketType clientSocket);

        unsigned short port_;
        SocketType serverSocket_;
        std::atomic<bool> running_;
        std::vector<std::thread> clientThreads_;

        NetworkInitializer netInit_;
    };

}
