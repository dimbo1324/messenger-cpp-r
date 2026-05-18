#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "Database.h"
#include "SessionRegistry.h"
#include "tcp/SocketOps.h"

#include <cstddef>
#include <memory>
#include <mutex>
#include <string>

class ClientHandler
{
public:
    ClientHandler(tcp::SocketHandle clientSocket,
                  std::shared_ptr<Database> db,
                  std::shared_ptr<SessionRegistry> sessions,
                  std::size_t maxFrameSize);
    ~ClientHandler();

    void run();
    bool deliverFrame(const std::string &payload);
    void disconnectFromAdmin();

private:
    bool handleRegister(const std::string &login, const std::string &pass);
    bool handleLogin(const std::string &login, const std::string &pass);
    void handleLogout();
    void handleList();
    void handleInbox();
    void handleMessage(const std::string &to, const std::string &text);
    void handleHistory(const std::string &target, int limit, int offset);
    void handleAllMessages(int limit);
    void handleBan(const std::string &login);
    void handleUnban(const std::string &login);
    void handleKick(const std::string &login);

    bool sendResponse(const std::string &payload);
    bool requireAuth();
    bool requireModerator();
    void cleanupSession();

    tcp::SocketHandle clientSocket_;
    std::shared_ptr<Database> db_;
    std::shared_ptr<SessionRegistry> sessions_;
    std::size_t maxFrameSize_;
    std::mutex sendMutex_;
    bool authed_{false};
    int userId_{-1};
    std::string username_;
    std::string role_;
};

#endif
