#pragma once
#include "tcp/ISocket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
namespace tcp
{
    class TcpSocketLinux : public ISocket
    {
    public:
        TcpSocketLinux();
        ~TcpSocketLinux() override;
        bool connect(const std::string &host, int port) override;
        std::size_t send(const char *data, std::size_t length) override;
        std::size_t receive(char *buffer, std::size_t maxlen) override;
        void close() override;

    private:
        int sockfd_;
    };
}