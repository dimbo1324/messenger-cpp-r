#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include "NetApp.h"
#include "ChatApp.h"

class Server
{
private:
    std::unordered_map<std::string, std::shared_ptr<ChatApp::User>> usersByLogin_;
    std::unordered_map<std::string, std::shared_ptr<ChatApp::User>> usersByName_;
    std::vector<ChatApp::Message> messages_;

public:
    void start()
    {
        NetApp::ServerSocket server(8080);
        std::cout << "Сервер запущен, ожидание подключений...\n";
        while (true)
        {
            auto clientSocket = server.acceptConnection();
            std::string request = server.receiveMessage(clientSocket);
            std::string response = processRequest(clientSocket, request);
            server.sendMessage(clientSocket, response);
        }
    }

private:
    std::string Server::processRequest(NetApp::SocketType clientSocket, const std::string &request)
    {
        std::istringstream iss(request);
        std::string command;
        iss >> command;

        if (command == "LOGIN")
        {
            std::string login, password;
            iss >> login >> password;
            if (usersByLogin_.count(login) && usersByLogin_[login]->getPassword() == password)
            {
                clientToUser_[clientSocket] = login; // Сохраняем, кто подключился
                return "LOGIN_OK " + login;
            }
            return "ERROR Invalid login or password";
        }
        else if (command == "SIGNUP")
        {
            std::string login, password, name;
            iss >> login >> password >> name;
            if (usersByLogin_.count(login) || usersByName_.count(name))
            {
                return "ERROR User already exists";
            }
            auto user = std::make_shared<ChatApp::User>(login, password, name);
            usersByLogin_[login] = user;
            usersByName_[name] = user;
            return "SIGNUP_OK";
        }
        else if (command == "GET_CHAT")
        {
            std::string response = "CHAT_MESSAGES ";
            for (const auto &msg : messages_)
            {
                response += msg.getFrom() + " -> " + msg.getTo() + ": " + msg.getText() + "\n";
            }
            return response;
        }
        else if (command == "SEND")
        {
            std::string recipient, text;
            iss >> recipient;
            std::getline(iss, text);
            std::string sender = clientToUser_[clientSocket];
            messages_.emplace_back(sender, recipient, text);
            return "OK Message sent";
        }
        else if (command == "GET_USERS")
        {
            std::string response = "USER_LIST ";
            for (const auto &pair : usersByName_)
            {
                response += pair.first + "\n";
            }
            return response;
        }
        return "ERROR Unknown command";
    }
};

int main()
{
    Server server;
    server.start();
    return 0;
}