#if !defined(_WIN32)
#include "tcp/TcpSocketLinux.h"
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <netdb.h>
namespace tcp
{
    TcpSocketLinux::TcpSocketLinux() : sockfd_(-1) {}
    TcpSocketLinux::~TcpSocketLinux()
    {
        close();
    }
    bool TcpSocketLinux::connect(const std::string &host, int port)
    {
        if (sockfd_ != -1)
        {
            this->close();
        }
        sockfd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0)
        {
            return false;
        }
#if defined(__APPLE__) || defined(__FreeBSD__)
        int set = 1;
        if (setsockopt(sockfd_, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int)) == -1)
        {
            ::close(sockfd_);
            sockfd_ = -1;
            return false;
        }
#endif
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        addrinfo *result = nullptr;
        const std::string service = std::to_string(port);
        if (::getaddrinfo(host.c_str(), service.c_str(), &hints, &result) != 0)
        {
            ::close(sockfd_);
            sockfd_ = -1;
            return false;
        }

        bool connected = false;
        for (addrinfo *rp = result; rp != nullptr; rp = rp->ai_next)
        {
            if (::connect(sockfd_, rp->ai_addr, rp->ai_addrlen) == 0)
            {
                connected = true;
                break;
            }
        }
        ::freeaddrinfo(result);

        if (!connected)
        {
            ::close(sockfd_);
            sockfd_ = -1;
            return false;
        }
        return true;
    }
    std::size_t TcpSocketLinux::send(const char *data, std::size_t length)
    {
        if (sockfd_ < 0 || data == nullptr || length == 0)
        {
            return 0;
        }
        int flags = 0;
#ifdef MSG_NOSIGNAL
        flags |= MSG_NOSIGNAL;
#endif
        ssize_t bytes_sent = ::send(sockfd_, data, length, flags);
        if (bytes_sent < 0)
        {
            return 0;
        }
        return static_cast<std::size_t>(bytes_sent);
    }
    std::size_t TcpSocketLinux::receive(char *buffer, std::size_t maxlen)
    {
        if (sockfd_ < 0 || buffer == nullptr || maxlen == 0)
        {
            return 0;
        }
        ssize_t bytes_received = ::recv(sockfd_, buffer, maxlen, 0);
        if (bytes_received < 0)
        {
            return 0;
        }
        return static_cast<std::size_t>(bytes_received);
    }
    void TcpSocketLinux::close()
    {
        if (sockfd_ >= 0)
        {
            if (::shutdown(sockfd_, SHUT_RDWR) < 0)
            {
            }
            ::close(sockfd_);
            sockfd_ = -1;
        }
    }
}
#endif
