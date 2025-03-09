#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#include "../ChatServer/ChatServer.h"

namespace ClientHandler
{
    void handleClient(int clientSocket, ChatServer &server);
}