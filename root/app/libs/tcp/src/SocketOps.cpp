#include "tcp/SocketOps.h"

#include <array>
#include <climits>
#include <cstdint>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <cerrno>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace
{
    std::uint32_t readBigEndian32(const char *data)
    {
        const auto b0 = static_cast<unsigned char>(data[0]);
        const auto b1 = static_cast<unsigned char>(data[1]);
        const auto b2 = static_cast<unsigned char>(data[2]);
        const auto b3 = static_cast<unsigned char>(data[3]);
        return (static_cast<std::uint32_t>(b0) << 24) |
               (static_cast<std::uint32_t>(b1) << 16) |
               (static_cast<std::uint32_t>(b2) << 8) |
               static_cast<std::uint32_t>(b3);
    }

#ifdef _WIN32
    int sendNative(tcp::SocketHandle socket, const char *data, std::size_t length)
    {
        if (length > static_cast<std::size_t>(INT_MAX))
        {
            length = static_cast<std::size_t>(INT_MAX);
        }
        return ::send(socket, data, static_cast<int>(length), 0);
    }

    int recvNative(tcp::SocketHandle socket, char *data, std::size_t length)
    {
        if (length > static_cast<std::size_t>(INT_MAX))
        {
            length = static_cast<std::size_t>(INT_MAX);
        }
        return ::recv(socket, data, static_cast<int>(length), 0);
    }
#else
    ssize_t sendNative(tcp::SocketHandle socket, const char *data, std::size_t length)
    {
        int flags = 0;
#ifdef MSG_NOSIGNAL
        flags |= MSG_NOSIGNAL;
#endif
        return ::send(socket, data, length, flags);
    }

    ssize_t recvNative(tcp::SocketHandle socket, char *data, std::size_t length)
    {
        return ::recv(socket, data, length, 0);
    }
#endif
}

namespace tcp
{
    bool isValidSocket(SocketHandle socket)
    {
        return socket != kInvalidSocket;
    }

    void closeSocket(SocketHandle socket)
    {
        if (!isValidSocket(socket))
        {
            return;
        }
#ifdef _WIN32
        ::shutdown(socket, SD_BOTH);
        ::closesocket(socket);
#else
        ::shutdown(socket, SHUT_RDWR);
        ::close(socket);
#endif
    }

    bool setReceiveTimeout(SocketHandle socket, int seconds)
    {
        if (!isValidSocket(socket) || seconds <= 0)
        {
            return false;
        }
#ifdef _WIN32
        DWORD timeoutMs = static_cast<DWORD>(seconds * 1000);
        return ::setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO,
                            reinterpret_cast<const char *>(&timeoutMs),
                            sizeof(timeoutMs)) == 0;
#else
        timeval timeout{};
        timeout.tv_sec = seconds;
        return ::setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO,
                            &timeout, sizeof(timeout)) == 0;
#endif
    }

    bool setReuseAddress(SocketHandle socket)
    {
        int enabled = 1;
#ifdef _WIN32
        return ::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR,
                            reinterpret_cast<const char *>(&enabled),
                            sizeof(enabled)) == 0;
#else
        return ::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR,
                            &enabled, sizeof(enabled)) == 0;
#endif
    }

    bool setNoSigPipe(SocketHandle socket)
    {
        if (!isValidSocket(socket))
        {
            return false;
        }
#if defined(__APPLE__) || defined(__FreeBSD__)
        int enabled = 1;
        return ::setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE,
                            &enabled, sizeof(enabled)) == 0;
#else
        return true;
#endif
    }

    bool sendAll(SocketHandle socket, const char *data, std::size_t length)
    {
        if (!isValidSocket(socket) || data == nullptr)
        {
            return false;
        }

        std::size_t sent = 0;
        while (sent < length)
        {
            const auto n = sendNative(socket, data + sent, length - sent);
            if (n <= 0)
            {
                return false;
            }
            sent += static_cast<std::size_t>(n);
        }
        return true;
    }

    bool sendFrame(SocketHandle socket, const std::string &payload, std::size_t maxFrameSize)
    {
        if (payload.size() > maxFrameSize)
        {
            return false;
        }
        const auto frame = encodeFrame(payload);
        return sendAll(socket, frame.data(), frame.size());
    }

    ReceiveFrameStatus receiveFrame(SocketHandle socket,
                                    std::string &payload,
                                    std::size_t maxFrameSize)
    {
        payload.clear();
        std::array<char, kFrameHeaderSize> header{};
        std::size_t received = 0;
        while (received < header.size())
        {
            const auto n = recvNative(socket, header.data() + received, header.size() - received);
            if (n == 0)
            {
                return received == 0 ? ReceiveFrameStatus::Closed : ReceiveFrameStatus::Error;
            }
            if (n < 0)
            {
                return ReceiveFrameStatus::Error;
            }
            received += static_cast<std::size_t>(n);
        }

        const std::uint32_t payloadSize = readBigEndian32(header.data());
        if (payloadSize > maxFrameSize)
        {
            return ReceiveFrameStatus::TooLarge;
        }

        payload.resize(payloadSize);
        received = 0;
        while (received < payload.size())
        {
            const auto n = recvNative(socket, &payload[received], payload.size() - received);
            if (n <= 0)
            {
                return n == 0 ? ReceiveFrameStatus::Closed : ReceiveFrameStatus::Error;
            }
            received += static_cast<std::size_t>(n);
        }

        return ReceiveFrameStatus::Ok;
    }
}
