#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <thread>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include "Database.h"

class ClientHandler
{
public:
    explicit ClientHandler(int clientSocket, std::shared_ptr<Database> db);
    ~ClientHandler();

    void run();

    int getUserId(const std::string &login);

    static std::mutex mtx_;

private:
    bool handleRegister(const std::string &login, const std::string &pass);
    bool handleLogin(const std::string &login, const std::string &pass);
    void handleInbox(const std::string &login);
    void handleMessage(const std::string &from, const std::string &to, const std::string &text);
    void handleHistory(const std::string &login, const std::string &target);
    void sendLine(int sock, const std::string &line);

    int clientSocket_;
    std::shared_ptr<Database> db_;
};

#endif
