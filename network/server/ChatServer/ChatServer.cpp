#include "ChatServer.h"
#include "../Chat/utils/Exceptions.h"
#include <sstream>
#include <mutex>
#include <stdexcept>
#include <iostream>
#include <algorithm>

#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

namespace ChatApp
{
    std::string ChatServer::hashPassword(const std::string &password)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char *>(password.c_str()), password.size(), hash);
        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    bool ChatServer::registerUser(const std::string &login, const std::string &password, const std::string &name)
    {
        if (login.empty() || password.empty() || name.empty())
            throw std::invalid_argument("Пустые аргументы не допускаются");

        std::lock_guard<std::mutex> lock(usersMutex);
        if (_usersByLogin.find(login) != _usersByLogin.end() || login == "всем")
        {
            throw UserLoginException();
        }
        if (_usersByName.find(name) != _usersByName.end() || name == "всем")
        {
            throw UserNameException();
        }
        std::string hashedPass = hashPassword(password);
        auto newUser = std::make_shared<User>(login, hashedPass, name);
        _usersByLogin[login] = newUser;
        _usersByName[name] = newUser;
        return true;
    }

    std::shared_ptr<User> ChatServer::authenticate(const std::string &login, const std::string &password)
    {
        std::lock_guard<std::mutex> lock(usersMutex);
        auto it = _usersByLogin.find(login);
        if (it != _usersByLogin.end())
        {
            std::string hashedInput = hashPassword(password);
            if (it->second->GetUserPassword() == hashedInput)
            {
                return it->second;
            }
        }
        return nullptr;
    }

    void ChatServer::addMessage(const std::string &from, const std::string &to, const std::string &text)
    {
        if (from.empty() || to.empty() || text.empty())
        {
            throw std::invalid_argument("Пустые строки недопустимы в сообщении");
        }
        {
            std::lock_guard<std::mutex> lock(messagesMutex);
            _messages.emplace_back(from, to, text);
        }
        json messageJson;
        messageJson["from"] = from;
        messageJson["to"] = to;
        messageJson["text"] = text;
        broadcastMessage(messageJson);
    }

    void ChatServer::broadcastMessage(const json &messageJson)
    {
        std::lock_guard<std::mutex> lock(activeClientsMutex);
        std::string messageStr = messageJson.dump();
        for (int clientSocket : _activeClientSockets)
        {
            ssize_t sent = send(clientSocket, messageStr.c_str(), messageStr.size(), 0);
            if (sent == -1)
            {
                std::cerr << "Ошибка отправки сообщения на сокет " << clientSocket
                          << ": " << strerror(errno) << std::endl;
            }
        }
    }

    std::string ChatServer::handleRequest(const std::string &request)
    {
        try
        {
            json reqJson = json::parse(request);
            if (!reqJson.contains("command"))
                return "ERROR:Отсутствует команда";

            std::string command = reqJson["command"];

            if (command == "REGISTER")
            {
                if (!reqJson.contains("login") || !reqJson.contains("password") || !reqJson.contains("name"))
                    return "ERROR:Неверный формат запроса";
                std::string login = reqJson["login"];
                std::string password = reqJson["password"];
                std::string name = reqJson["name"];
                if (login.empty() || password.empty() || name.empty())
                    return "ERROR:Пустые поля недопустимы";

                registerUser(login, password, name);
                return "OK";
            }
            else if (command == "LOGIN")
            {
                if (!reqJson.contains("login") || !reqJson.contains("password"))
                    return "ERROR:Неверный формат запроса";
                std::string login = reqJson["login"];
                std::string password = reqJson["password"];
                if (login.empty() || password.empty())
                    return "ERROR:Пустые поля недопустимы";

                auto user = authenticate(login, password);
                return user ? "OK" : "ERROR:Неверный логин или пароль";
            }
            else if (command == "MESSAGE")
            {
                if (!reqJson.contains("from") || !reqJson.contains("to") || !reqJson.contains("text"))
                    return "ERROR:Неверный формат запроса";
                std::string from = reqJson["from"];
                std::string to = reqJson["to"];
                std::string text = reqJson["text"];
                if (from.empty() || to.empty() || text.empty())
                    return "ERROR:Пустые поля недопустимы";

                addMessage(from, to, text);
                return "OK";
            }
            else if (command == "GET_MESSAGES")
            {
                json messagesJson;
                {
                    std::lock_guard<std::mutex> lock(messagesMutex);
                    messagesJson["messages"] = json::array();
                    for (const auto &msg : _messages)
                    {
                        json msgJson;
                        msgJson["from"] = msg.GetFrom();
                        msgJson["to"] = msg.GetTo();
                        msgJson["text"] = msg.GetText();
                        messagesJson["messages"].push_back(msgJson);
                    }
                }
                return messagesJson.dump();
            }
            else
            {
                return "ERROR:Неизвестная команда";
            }
        }
        catch (json::parse_error &e)
        {
            return "ERROR:Неверный формат JSON";
        }
        catch (const std::exception &e)
        {
            return std::string("ERROR:") + e.what();
        }
    }

    void ChatServer::addActiveClient(int socket)
    {
        std::lock_guard<std::mutex> lock(activeClientsMutex);
        if (std::find(_activeClientSockets.begin(), _activeClientSockets.end(), socket) == _activeClientSockets.end())
            _activeClientSockets.push_back(socket);
    }

    void ChatServer::removeActiveClient(int socket)
    {
        std::lock_guard<std::mutex> lock(activeClientsMutex);
        _activeClientSockets.erase(std::remove(_activeClientSockets.begin(), _activeClientSockets.end(), socket),
                                   _activeClientSockets.end());
    }
}
