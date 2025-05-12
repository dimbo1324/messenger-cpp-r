#include "tcp/ISocket.h"
#include "tcp/TcpSocketLinux.h"
#include <cstring>
#include <stdexcept>

namespace tcp
{

    TcpSocketLinux::TcpSocketLinux() : sockfd_(-1) {}

    TcpSocketLinux::~TcpSocketLinux()
    {
        close();
    }

    bool TcpSocketLinux::connect(const std::string &host, int port)
    {
        sockfd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0)
            return false;
        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (::inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0)
            return false;
        return (::connect(sockfd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0);
    }

    std::size_t TcpSocketLinux::send(const char *data, std::size_t length)
    {
        return ::send(sockfd_, data, length, 0);
    }

    std::size_t TcpSocketLinux::receive(char *buffer, std::size_t maxlen)
    {
        ssize_t bytes = ::recv(sockfd_, buffer, static_cast<int>(maxlen), 0);
        return bytes > 0 ? static_cast<std::size_t>(bytes) : 0;
    }

    void TcpSocketLinux::close()
    {
        if (sockfd_ >= 0)
        {
            ::shutdown(sockfd_, SHUT_RDWR);
            ::close(sockfd_);
            sockfd_ = -1;
        }
    }

}