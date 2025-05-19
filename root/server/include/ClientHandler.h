#pragma once
#include <thread>
#include <string>
#include <libpq-fe.h>
class ClientHandler
{
public:
    ClientHandler(int clientSocket, PGconn *dbConn);
    ~ClientHandler();

private:
    void run();
    void handleRegister(const std::string &login, const std::string &pass);
    void handleLogin(const std::string &login, const std::string &pass);
    void handleListUsers();
    void handleInbox(const std::string &login);
    void handleMessage(const std::string &from,
                       const std::string &to,
                       const std::string &text);
    void handleHistory(const std::string &a, const std::string &b);
    void sendLine(int sock, const std::string &line);
    int clientSocket_;
    PGconn *dbConn_;
    std::thread handlerThread_;
    std::string username_;
    bool authed_{false};
};
