#include "ClientHandler.h"
#include "Logger.h"
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#endif
#include <cstring>
#include <string>
#include <algorithm>
ClientHandler::ClientHandler(int socket)
    : clientSocket(socket),
      handlerThread(&ClientHandler::run, this)
{
    handlerThread.detach();
}
ClientHandler::~ClientHandler()
{
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}
void ClientHandler::run()
{
    char buffer[1024];
    bool loggedIn = false;
    std::string username;
    Logger::getInstance().log("Клиент подключён: socket=" + std::to_string(clientSocket));
    while (true)
    {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0)
        {
            Logger::getInstance().log("Клиент отключён: socket=" + std::to_string(clientSocket));
            break;
        }
        std::string msg(buffer, bytes);
        Logger::getInstance().log("Получено от " + std::to_string(clientSocket) + ": " + msg);
        if (!msg.empty() && msg.back() == '\n')
            msg.pop_back();
        if (!loggedIn)
        {
            const std::string prefix = "LOGIN ";
            if (msg.rfind(prefix, 0) == 0)
            {
                auto spacePos = msg.find(' ', prefix.size());
                if (spacePos != std::string::npos)
                {
                    username = msg.substr(prefix.size(), spacePos - prefix.size());
                }
                else
                {
                    username = msg.substr(prefix.size());
                }
                const char *ok = "LOGIN_OK\n";
                send(clientSocket, ok, strlen(ok), 0);
                Logger::getInstance().log("Пользователь вошёл: " + username);
                loggedIn = true;
                continue;
            }
            else
            {
                const char *err = "LOGIN_ERROR\n";
                send(clientSocket, err, strlen(err), 0);
                continue;
            }
        }
        else
        {
            const std::string mtx = "MESSAGE ";
            if (msg.rfind(mtx, 0) == 0)
            {
                std::string echo = "MESSAGE_OK " + msg.substr(mtx.size()) + "\n";
                send(clientSocket, echo.c_str(), echo.size(), 0);
                Logger::getInstance().log("Эхо для " + username + ": " + echo);
            }
            else
            {
                const char *err = "UNKNOWN_CMD\n";
                send(clientSocket, err, strlen(err), 0);
            }
        }
    }
}
