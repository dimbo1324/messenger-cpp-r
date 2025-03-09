#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <limits.h>
#endif

namespace ChatApp
{
    class ClientConnection
    {
    private:
        int _socket;
        sockaddr_in _serverAddress;
        std::atomic<bool> _stop;
        std::thread _receiveThread;
        std::mutex _socketMutex;

        void receiveMessages();

    public:
        ClientConnection(const std::string &ip, int port);
        ~ClientConnection();

        bool isConnected() const { return _socket != -1; }

        std::string sendRequest(const std::string &request);
    };
}

#endif
