#pragma once
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

namespace ChatApp {
    class ClientConnection {
    private:
        int _socket;
        sockaddr_in _serverAddress;

    public:
        ClientConnection(const std::string& ip, int port);
        ~ClientConnection();
        std::string sendRequest(const std::string& request);
    };
}