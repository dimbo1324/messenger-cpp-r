#pragma once
#include "tcp/ISocket.h"
#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif
namespace tcp
{
    class TcpSocketWin : public ISocket
    {
    public:
        TcpSocketWin();
        ~TcpSocketWin() override;
        bool connect(const std::string &host, int port) override;
        std::size_t send(const char *data, std::size_t length) override;
        std::size_t receive(char *buffer, std::size_t maxlen) override;
        void close() override;

    private:
#if defined(_WIN32)
        SOCKET sock_;
        bool initialized_;
#endif
    };
}