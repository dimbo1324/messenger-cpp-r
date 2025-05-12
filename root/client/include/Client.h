#pragma once
#include <string>
#include <memory>
#include <thread>
#include "tcp/ISocket.h"
class Client
{
public:
    Client(const std::string &host, unsigned short port);
    ~Client();
    void run();

private:
    void connectToServer();
    void login();
    void sendMessage();
    void receiveLoop();
    std::unique_ptr<tcp::ISocket> socket_;
    std::thread recvThread_;
    bool running_{false};
    std::string serverHost_;
    unsigned short serverPort_;
    std::string currentUser_;
};