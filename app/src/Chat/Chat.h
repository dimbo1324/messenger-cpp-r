#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "../Message/Message.h"
#include "../User/User.h"

namespace ChatApp
{
    class Chat
    {
    private:
        bool _isActive = false;
        std::unordered_map<std::string, std::shared_ptr<User>> _usersByLogin;
        std::unordered_map<std::string, std::shared_ptr<User>> _usersByName;
        std::vector<Message> _messages;
        std::shared_ptr<User> _currentUser = nullptr;

        void login();

        void signUp();

        void displayChat() const;

        void displayAllUserNames() const;

        void addMessage();

        std::shared_ptr<User> findUserByLogin(const std::string &login) const;

        std::shared_ptr<User> findUserByName(const std::string &name) const;

    public:
        Chat() = default;

        ~Chat() = default;

        void Start();

        bool ChatIsActive() const { return _isActive; }
        std::shared_ptr<User> GetCurrentUser() const { return _currentUser; }

        void displayLoginMenu();

        void displayUserMenu();
    };
}
