#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <iostream>
#include <map>

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
    class Server
    {
    private:
        unsigned short port_;
        SocketType serverSocket_;
        std::atomic<bool> running_;
        std::vector<std::thread> clientThreads_;
        std::mutex dataMutex_;
        std::map<SocketType, std::string> clientToUser_;

    public:
        Server(unsigned short port);
        ~Server();
        bool start();
        void stop();
        void acceptClients();
        void handleClient(SocketType clientSocket);
        std::string processRequest(SocketType clientSocket, const std::string &request);
        void handleDisconnect(SocketType clientSocket);
        void broadcastMessage(const std::string &message);
    };

    class Client
    {
    private:
        std::string serverAddress_;
        unsigned short serverPort_;
        SocketType clientSocket_;
        std::atomic<bool> connected_;
        std::atomic<bool> receiving_;
        std::thread receiveThread_;

    public:
        Client(const std::string &serverAddress, unsigned short serverPort);
        ~Client();
        bool connectToServer();
        void disconnect();
        bool sendMessage(const std::string &message);
        void startReceiving();
        void stopReceiving();
        void receiveLoop();
        bool isConnected() const;
    };
}