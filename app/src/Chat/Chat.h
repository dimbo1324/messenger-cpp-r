#pragma once
#include <string>
#include <memory>
#include "../Message/Message.h"
#include "../User/User.h"
#include "../../networking/client/Client.h"
namespace ChatApp
{
class Chat
{
private:
bool _isActive = false;
std::shared_ptr<User> _currentUser = nullptr;
NetApp::Client client_;
void login();
void signUp();
void displayChat();
void displayAllUserNames();
void addMessage();
public:
Chat(const std::string &serverAddress, unsigned short serverPort);
~Chat() = default;
void Start();
bool ChatIsActive() const { return _isActive; }
std::shared_ptr<User> GetCurrentUser() const { return _currentUser; }
void setCurrentUser(const std::shared_ptr<User> &user) { _currentUser = user; }
void displayLoginMenu();
void displayUserMenu();
};
}
