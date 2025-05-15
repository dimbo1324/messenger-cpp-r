#pragma once
#include "tcp/ISocket.h"
#include <memory>
#include <thread>
namespace server
{
    class ClientHandler
    {
    public:
        ClientHandler(std::unique_ptr<tcp::ISocket> socket);
        ~ClientHandler();
        void start();
        void join();

    private:
        void run();
        std::unique_ptr<tcp::ISocket> socket_;
        std::thread thread_;
        bool running_{false};
    };
}
