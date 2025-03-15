#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include "../chatApp/ChatApp.h"

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET SocketType;
const SocketType INVALID_SOCKET_VALUE = INVALID_SOCKET;
const int SOCKET_ERROR_VALUE = SOCKET_ERROR;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int SocketType;
const SocketType INVALID_SOCKET_VALUE = -1;
const int SOCKET_ERROR_VALUE = -1;
#endif

namespace NetApp
{
    class Chat;
    class Client
    {
    private:
        std::string serverAddress_;
        unsigned short serverPort_;
        SocketType clientSocket_;
        std::atomic<bool> connected_;
        std::atomic<bool> receiving_;
        std::thread receiveThread_;
        ChatApp::Chat *chatPtr_;

    public:
        Client(const std::string &serverAddress, unsigned short serverPort, ChatApp::Chat *chat);
        ~Client();
        bool connectToServer();
        void disconnect();
        bool sendMessage(const std::string &message);
        void startReceiving();
        void stopReceiving();
        void receiveLoop();
        bool isConnected() const;
        void handleServerResponse(const std::string &response);
    };
}