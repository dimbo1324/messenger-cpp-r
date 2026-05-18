#pragma once

#include "tcp/Protocol.h"

#include <cstddef>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
namespace tcp
{
    using SocketHandle = SOCKET;
    constexpr SocketHandle kInvalidSocket = INVALID_SOCKET;
}
#else
namespace tcp
{
    using SocketHandle = int;
    constexpr SocketHandle kInvalidSocket = -1;
}
#endif

namespace tcp
{
    bool isValidSocket(SocketHandle socket);
    void closeSocket(SocketHandle socket);
    bool setReceiveTimeout(SocketHandle socket, int seconds);
    bool setReuseAddress(SocketHandle socket);
    bool setNoSigPipe(SocketHandle socket);

    bool sendAll(SocketHandle socket, const char *data, std::size_t length);
    bool sendFrame(SocketHandle socket, const std::string &payload);
    ReceiveFrameStatus receiveFrame(SocketHandle socket,
                                    std::string &payload,
                                    std::size_t maxFrameSize = kDefaultMaxFrameSize);
}
