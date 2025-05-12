#if defined(_WIN32)
#include "tcp/TcpSocketWin.h"
#include <stdexcept>
namespace tcp
{
    TcpSocketWin::TcpSocketWin() : sock_(INVALID_SOCKET), initialized_(false)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
        {
            initialized_ = true;
        }
        else
        {
        }
    }
    TcpSocketWin::~TcpSocketWin()
    {
        this->close();
        if (initialized_)
        {
            WSACleanup();
        }
    }
    bool TcpSocketWin::connect(const std::string &host, int port)
    {
        if (!initialized_)
        {
            return false;
        }
        if (sock_ != INVALID_SOCKET)
        {
            this->close();
        }
        sock_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock_ == INVALID_SOCKET)
        {
            return false;
        }
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(static_cast<USHORT>(port));
        if (::InetPtonA(AF_INET, host.c_str(), &addr.sin_addr) != 1)
        {
            ::closesocket(sock_);
            sock_ = INVALID_SOCKET;
            return false;
        }
        if (::connect(sock_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == SOCKET_ERROR)
        {
            ::closesocket(sock_);
            sock_ = INVALID_SOCKET;
            return false;
        }
        return true;
    }
    std::size_t TcpSocketWin::send(const char *data, std::size_t length)
    {
        if (sock_ == INVALID_SOCKET || !initialized_ || data == nullptr || length == 0)
        {
            return 0;
        }
        if (length > static_cast<std::size_t>(INT_MAX))
        {
            return 0;
        }
        int bytes_sent = ::send(sock_, data, static_cast<int>(length), 0);
        if (bytes_sent == SOCKET_ERROR)
        {
            return 0;
        }
        return static_cast<std::size_t>(bytes_sent);
    }
    std::size_t TcpSocketWin::receive(char *buffer, std::size_t maxlen)
    {
        if (sock_ == INVALID_SOCKET || !initialized_ || buffer == nullptr || maxlen == 0)
        {
            return 0;
        }
        if (maxlen > static_cast<std::size_t>(INT_MAX))
        {
            return 0;
        }
        int bytes_received = ::recv(sock_, buffer, static_cast<int>(maxlen), 0);
        if (bytes_received == SOCKET_ERROR)
        {
            return 0;
        }
        return static_cast<std::size_t>(bytes_received);
    }
    void TcpSocketWin::close()
    {
        if (sock_ != INVALID_SOCKET)
        {
            if (::shutdown(sock_, SD_BOTH) == SOCKET_ERROR)
            {
            }
            ::closesocket(sock_);
            sock_ = INVALID_SOCKET;
        }
    }
}
#endif