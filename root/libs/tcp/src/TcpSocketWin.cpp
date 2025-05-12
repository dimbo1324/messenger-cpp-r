#include "tcp/TcpSocketWin.h"
#include <stdexcept>

namespace tcp
{

    TcpSocketWin::TcpSocketWin()
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
        {
            initialized_ = true;
        }
    }

    TcpSocketWin::~TcpSocketWin()
    {
        close();
        if (initialized_)
            WSACleanup();
    }

    bool TcpSocketWin::connect(const std::string &host, int port)
    {
        if (!initialized_)
            return false;
        sock_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock_ == INVALID_SOCKET)
            return false;
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (::InetPtonA(AF_INET, host.c_str(), &addr.sin_addr) != 1)
        {
            closesocket(sock_);
            return false;
        }
        return (::connect(sock_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0);
    }

    std::size_t TcpSocketWin::send(const char *data, std::size_t length)
    {
        int res = ::send(sock_, data, static_cast<int>(length), 0);
        return res == SOCKET_ERROR ? 0 : static_cast<std::size_t>(res);
    }

    std::size_t TcpSocketWin::receive(char *buffer, std::size_t maxlen)
    {
        int rec = ::recv(sock_, buffer, static_cast<int>(maxlen), 0);
        return rec <= 0 ? 0 : static_cast<std::size_t>(rec);
    }

    void TcpSocketWin::close()
    {
        if (sock_ != INVALID_SOCKET)
        {
            ::shutdown(sock_, SD_BOTH);
            ::closesocket(sock_);
            sock_ = INVALID_SOCKET;
        }
    }

}