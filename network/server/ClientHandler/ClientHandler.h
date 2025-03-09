#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

namespace ClientHandler
{
    void handleClient(int clientSocket);
}
