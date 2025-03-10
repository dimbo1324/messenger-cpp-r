#pragma once
#include <string>
#include <thread>
#include <atomic>
#include "SocketHandler.h"
#include "NetworkInitializer.h"

namespace Networking
{

    class Client
    {
    public:
        Client(const std::string &serverAddress, unsigned short serverPort);
        ~Client();

        bool connectToServer();
        void disconnect();
        bool sendMessage(const std::string &message);
        void startReceiving();
        void stopReceiving();
        bool isConnected() const;

    private:
        void receiveLoop();

        std::string serverAddress_;
        unsigned short serverPort_;
        SocketType clientSocket_;
        std::atomic<bool> connected_;
        std::atomic<bool> receiving_;
        std::thread receiveThread_;

        NetworkInitializer netInit_;
    };

}
