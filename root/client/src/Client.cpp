#include "Client.h"
#include "UI.h"
#include "tcp/SocketFactory.h"
#include <iostream>
Client::Client(const std::string &host, unsigned short port)
    : serverHost_(host), serverPort_(port) {}
Client::~Client()
{
    running_ = false;
    if (recvThread_.joinable())
        recvThread_.join();
}
void Client::connectToServer()
{
    socket_ = tcp::SocketFactory::create();
    socket_->connect(serverHost_, serverPort_);
}
void Client::login()
{
    auto creds = UI::promptLogin();
    socket_->send(creds);
    currentUser_ = creds.substr(0, creds.find(' '));
    std::cout << "Вход выполнен как " << currentUser_ << "\n";
}
void Client::sendMessage()
{
    auto msg = UI::promptMessage();
    std::string packet = "SEND " + msg;
    socket_->send(packet);
}
void Client::receiveLoop()
{
    while (running_)
    {
        auto data = socket_->recv();
        if (!data.empty())
        {
            std::cout << data << std::endl;
        }
    }
}
void Client::run()
{
    connectToServer();
    running_ = true;
    recvThread_ = std::thread(&Client::receiveLoop, this);
    while (true)
    {
        char cmd = UI::showLoginMenu();
        if (cmd == 'l')
        {
            login();
            break;
        }
        if (cmd == 'q')
        {
            running_ = false;
            return;
        }
    }
    while (running_)
    {
        char cmd = UI::showUserMenu();
        if (cmd == 's')
        {
            sendMessage();
        }
        else if (cmd == 'o')
        {
            running_ = false;
        }
    }
}
