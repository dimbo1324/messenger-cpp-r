#pragma once
#include <stdexcept>

#if defined(_WIN32)
#include <winsock2.h>
typedef SOCKET SocketType;
#define INVALID_SOCKET_VALUE INVALID_SOCKET
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int SocketType;
#define INVALID_SOCKET_VALUE (-1)
#define SOCKET_ERROR (-1)
#endif

class SocketHandler
{
public:
    explicit SocketHandler(SocketType sock = INVALID_SOCKET_VALUE) : socket_(sock) {}
    ~SocketHandler() { closeSocket(); }
    SocketType get() const { return socket_; }
    void reset(SocketType sock = INVALID_SOCKET_VALUE)
    {
        closeSocket();
        socket_ = sock;
    }
    void closeSocket()
    {
        if (socket_ != INVALID_SOCKET_VALUE)
        {
#if defined(_WIN32)
            closesocket(socket_);
#else
            close(socket_);
#endif
            socket_ = INVALID_SOCKET_VALUE;
        }
    }

private:
    SocketType socket_;
};
