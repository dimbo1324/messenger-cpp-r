#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <thread>
#include <string>
#include <map>
#include <vector>
#include <mutex>

class ClientHandler
{
public:
    explicit ClientHandler(int clientSocket);
    ~ClientHandler();

private:
    void run();

    int clientSocket_;
    std::thread handlerThread_;

public:
    static std::map<std::string, std::string> credentials_;
    static std::map<std::string, int> onlineClients_;
    static std::map<std::string, std::vector<std::string>> history_[2];
    static std::mutex mtx_;

private:
    bool handleRegister(const std::string &login, const std::string &pass);
    bool handleLogin(const std::string &login, const std::string &pass);
    void sendLine(int sock, const std::string &line);
};

#endif
