#pragma once

#include "tcp/ISocket.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

class Client
{
public:
    Client(const std::string &host, unsigned short port);
    ~Client();
    void run();

private:
    void connectToServer();
    void registerUser();
    void login();
    void logout();
    void listUsers();
    void inbox();
    void sendMessage();
    void history();
    void receiveLoop();
    bool sendCommand(const std::string &command);

    std::string currentUser();
    void setCurrentUser(const std::string &user);
    void clearCurrentUser();

    std::unique_ptr<tcp::ISocket> socket_;
    std::thread recvThread_;
    std::atomic<bool> running_{false};
    std::string serverHost_;
    unsigned short serverPort_;
    std::mutex userMutex_;
    std::string currentUser_;
    std::string pendingLogin_;
};
