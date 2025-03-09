#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

class ServerConnection
{
public:
    ServerConnection();
    ~ServerConnection();
    bool initialize();
    int acceptClient();

private:
    int serverSocket;
    sockaddr_in serverAddress;
};
