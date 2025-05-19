#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H
#include <thread>
#include <string>
#include <map>
#include <mutex>
class ClientHandler
{
public:
    explicit ClientHandler(int clientSocket);
    ~ClientHandler();
    static std::mutex mtx_;
    static std::map<std::string, std::string> credentials_;
    static std::map<std::string, int> onlineClients_;

private:
    void run();
    bool handleRegister(const std::string &login, const std::string &pass);
    bool handleLogin(const std::string &login, const std::string &pass);
    void handleInbox(const std::string &login);
    void sendLine(int sock, const std::string &line);
    int clientSocket_;
    std::thread handlerThread_;
};
#endif
