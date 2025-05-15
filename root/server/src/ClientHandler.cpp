#pragma once

#include <thread>
#include <memory>
#include "tcp/ISocket.h"

namespace server
{

    class ClientHandler
    {
    public:
        explicit ClientHandler(int clientSocketFd);
        ~ClientHandler();

        void start();
        void join();

    private:
        void run();

        int clientFd_;
        std::thread th_;
        bool running_{false};
    };

}
