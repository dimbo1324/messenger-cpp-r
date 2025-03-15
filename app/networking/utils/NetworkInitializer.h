#pragma once
#include <stdexcept>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

class NetworkInitializer
{
public:
    NetworkInitializer()
    {
#if defined(_WIN32)
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0)
        {
            throw std::runtime_error("WSAStartup failed with error: " + std::to_string(result));
        }
#endif
    }
    ~NetworkInitializer()
    {
#if defined(_WIN32)
        WSACleanup();
#endif
    }
};
