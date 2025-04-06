#pragma once

#include <string>
#include <memory>
#include "../Message/Message.h"
#include "../User/User.h"
#include "../../networking/client/Client.h"
#include "../../../db/DatabaseManager/DatabaseManager.h"

namespace ChatApp
{

    class Chat
    {
    private:
        DatabaseManager dbManager_;
        bool isActive_ = false;
        std::shared_ptr<User> currentUser_ = nullptr;
        NetApp::Client client_;

        int getValidatedChoice();

        void login();
        void signUp();
        void displayChat();
        void displayAllUserNames();
        void addMessage();

    public:
        Chat(const std::string &serverAddress, unsigned short serverPort);
        ~Chat();

        void start();
        bool isChatActive() const { return isActive_; }
        std::shared_ptr<User> getCurrentUser() const { return currentUser_; }
        void setCurrentUser(const std::shared_ptr<User> &user) { currentUser_ = user; }
        void displayLoginMenu();
        void displayUserMenu();
    };

}
