#pragma once
#include <memory>
#include "../User/User.h"
#include "ClientConnection/ClientConnection.h"

namespace ChatApp
{
    class ChatClient
    {
    private:
        std::shared_ptr<User> _currentUser;
        ClientConnection _connection;

        void login();
        void signUp();
        void displayChat();
        void addMessage();

    public:
        ChatClient();
        void displayLoginMenu();
        void displayUserMenu();
    };
}
