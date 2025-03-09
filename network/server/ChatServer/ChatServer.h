#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <string>
#include "../User/User.h"
#include "../Message/Message.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace ChatApp
{
    class ChatServer
    {
    private:
        std::unordered_map<std::string, std::shared_ptr<User>> _usersByLogin;
        std::unordered_map<std::string, std::shared_ptr<User>> _usersByName;
        std::vector<Message> _messages;
        std::mutex usersMutex;
        std::mutex messagesMutex;

        std::vector<int> _activeClientSockets;
        std::mutex activeClientsMutex;

        std::string hashPassword(const std::string &password);

        void broadcastMessage(const json &messageJson);

    public:
        bool registerUser(const std::string &login, const std::string &password, const std::string &name);
        std::shared_ptr<User> authenticate(const std::string &login, const std::string &password);
        void addMessage(const std::string &from, const std::string &to, const std::string &text);
        std::string handleRequest(const std::string &request);

        void addActiveClient(int socket);
        void removeActiveClient(int socket);
    };
}
