#include "ChatServer.h"
#include "../Chat/utils/Exceptions.h"
#include <sstream>
#include <mutex>

namespace ChatApp
{
    bool ChatServer::registerUser(const std::string &login, const std::string &password, const std::string &name)
    {
        std::lock_guard<std::mutex> lock(usersMutex);
        if (_usersByLogin.find(login) != _usersByLogin.end() || login == "всем")
        {
            throw UserLoginException();
        }
        if (_usersByName.find(name) != _usersByName.end() || name == "всем")
        {
            throw UserNameException();
        }
        auto newUser = std::make_shared<User>(login, password, name);
        _usersByLogin[login] = newUser;
        _usersByName[name] = newUser;
        return true;
    }

    std::shared_ptr<User> ChatServer::authenticate(const std::string &login, const std::string &password)
    {
        std::lock_guard<std::mutex> lock(usersMutex);
        auto user = _usersByLogin.find(login);
        if (user != _usersByLogin.end() && user->second->GetUserPassword() == password)
        {
            return user->second;
        }
        return nullptr;
    }

    void ChatServer::addMessage(const std::string &from, const std::string &to, const std::string &text)
    {
        std::lock_guard<std::mutex> lock(messagesMutex);
        _messages.emplace_back(from, to, text);
    }

    std::string ChatServer::handleRequest(const std::string &request)
    {
        std::istringstream iss(request);
        std::string command, arg1, arg2, arg3;
        std::getline(iss, command, ':');
        std::getline(iss, arg1, ':');
        std::getline(iss, arg2, ':');
        std::getline(iss, arg3);

        if (command == "REGISTER")
        {
            try
            {
                registerUser(arg1, arg2, arg3);
                return "OK";
            }
            catch (const std::exception &e)
            {
                return "ERROR:" + std::string(e.what());
            }
        }
        else if (command == "LOGIN")
        {
            auto user = authenticate(arg1, arg2);
            return user ? "OK" : "ERROR:Неверный логин или пароль";
        }
        else if (command == "MESSAGE")
        {
            addMessage(arg1, arg2, arg3);
            return "OK";
        }
        else if (command == "GET_MESSAGES")
        {
            std::lock_guard<std::mutex> lock(messagesMutex);
            std::string messages;
            for (const auto &msg : _messages)
            {
                messages += msg.GetFrom() + " -> " + msg.GetTo() + ": " + msg.GetText() + "\n";
            }
            return messages.empty() ? "Нет сообщений" : messages;
        }
        return "ERROR:Неизвестная команда";
    }
}