#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <memory>
#include <sstream>
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
    class Server
    {
    private:
        unsigned short port_;
        SocketType serverSocket_;
        std::atomic<bool> running_;
        std::vector<std::thread> clientThreads_;
        std::mutex dataMutex_;

        std::unordered_map<std::string, std::shared_ptr<ChatApp::User>> usersByLogin_;
        std::unordered_map<std::string, std::shared_ptr<ChatApp::User>> usersByName_;
        std::vector<ChatApp::Message> messages_;
        std::unordered_map<SocketType, std::string> clientToUser_;

    public:
        Server(unsigned short port);
        ~Server();
        bool start();
        void stop();
        void acceptClients();
        void handleClient(SocketType clientSocket);
        std::string processRequest(SocketType clientSocket, const std::string &request);

    private:
        void handleDisconnect(SocketType clientSocket);
    };
}