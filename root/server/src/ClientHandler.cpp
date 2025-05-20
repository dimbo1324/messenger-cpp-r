// src/ClientHandler.cpp
#include "ClientHandler.h"
#include "Logger.h"
#include <iostream> // для std::cerr, std::endl
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <utility>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#endif

std::mutex ClientHandler::mtx_;

ClientHandler::ClientHandler(int sock, std::shared_ptr<Database> db)
    : clientSocket_(sock), db_(db)
{
}

ClientHandler::~ClientHandler()
{
#ifdef _WIN32
    closesocket(clientSocket_);
#else
    close(clientSocket_);
#endif
}

void ClientHandler::sendLine(int sock, const std::string &line)
{
    std::string l = line + "\n";
    ::send(sock, l.c_str(), static_cast<int>(l.size()), 0);
}

bool ClientHandler::handleRegister(const std::string &login, const std::string &pass)
{
    if (db_->registerUser(login, pass))
    {
        sendLine(clientSocket_, "REGISTER_OK");
        return true;
    }
    else
    {
        sendLine(clientSocket_, "REGISTER_ERROR");
        return false;
    }
}

bool ClientHandler::handleLogin(const std::string &login, const std::string &pass)
{
    if (db_->loginUser(login, pass))
    {
        sendLine(clientSocket_, "LOGIN_OK");
        return true;
    }
    else
    {
        sendLine(clientSocket_, "LOGIN_ERROR");
        return false;
    }
}

void ClientHandler::handleInbox(const std::string &login)
{
    int userId = getUserId(login);
    if (userId == -1)
    {
        sendLine(clientSocket_, "ERROR_USER_NOT_FOUND");
        return;
    }
    std::vector<std::string> messages = db_->getInbox(userId);
    sendLine(clientSocket_, "INBOX_COUNT " + std::to_string(messages.size()));
    for (const auto &msg : messages)
    {
        sendLine(clientSocket_, "INBOX_MSG " + msg);
    }
    sendLine(clientSocket_, "INBOX_END");
}

void ClientHandler::handleMessage(const std::string &from, const std::string &to, const std::string &text)
{
    int senderId = getUserId(from);
    int receiverId = getUserId(to);
    if (senderId == -1 || receiverId == -1)
    {
        sendLine(clientSocket_, "ERROR_USER_NOT_FOUND");
        return;
    }
    db_->sendMessage(senderId, receiverId, text);
    sendLine(clientSocket_, "MESSAGE_OK");
}

void ClientHandler::handleHistory(const std::string &login, const std::string &target)
{
    int userId = getUserId(login);
    int targetId = getUserId(target);
    if (userId == -1 || targetId == -1)
    {
        sendLine(clientSocket_, "ERROR_USER_NOT_FOUND");
        return;
    }
    try
    {
        pqxx::work txn(db_->getConnection());
        pqxx::result res = txn.exec_params(
            "SELECT u1.login AS sender, u2.login AS receiver, m.text "
            "FROM chat.messages m "
            "JOIN chat.users u1 ON m.sender_id   = u1.id "
            "JOIN chat.users u2 ON m.receiver_id = u2.id "
            "WHERE (m.sender_id = $1 AND m.receiver_id = $2) "
            "   OR (m.sender_id = $2 AND m.receiver_id = $1)",
            userId, targetId);

        for (const auto &row : res)
        {
            std::string msg = "[" + row[0].as<std::string>() + " to " + row[1].as<std::string>() + "]: " + row[2].as<std::string>();
            sendLine(clientSocket_, "HIST " + msg);
        }
        sendLine(clientSocket_, "HISTORY_END");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Ошибка получения истории: " << e.what() << std::endl;
        sendLine(clientSocket_, "ERROR_DB");
    }
}

int ClientHandler::getUserId(const std::string &login)
{
    try
    {
        pqxx::work txn(db_->getConnection());
        pqxx::result res = txn.exec_params(
            "SELECT id FROM chat.users WHERE login = $1",
            login);
        if (!res.empty())
            return res[0][0].as<int>();
        else
            return -1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Ошибка получения ID пользователя: " << e.what() << std::endl;
        return -1;
    }
}

void ClientHandler::run()
{
    char buf[1024];
    std::string username;
    bool authed = false;

    Logger::getInstance().log("Клиент подключён socket=" + std::to_string(clientSocket_));

    while (true)
    {
        int n = recv(clientSocket_, buf, sizeof(buf) - 1, 0);
        if (n <= 0)
            break;
        buf[n] = '\0';

        std::string line(buf);
        if (!line.empty() && line.back() == '\n')
            line.pop_back();

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "REGISTER")
        {
            std::string u, p;
            iss >> u >> p;
            handleRegister(u, p);
        }
        else if (cmd == "LOGIN")
        {
            std::string u, p;
            iss >> u >> p;
            if (handleLogin(u, p))
            {
                authed = true;
                username = u;
                Logger::getInstance().log("User logged in: " + u);
            }
        }
        else if (!authed)
        {
            sendLine(clientSocket_, "ERROR_NOT_AUTH");
        }
        else if (cmd == "LIST")
        {
            try
            {
                pqxx::work txn(db_->getConnection());
                pqxx::result res = txn.exec(
                    "SELECT login FROM chat.users "
                    "WHERE id IN (SELECT user_id FROM chat.online_status WHERE status = 'онлайн')");

                std::string out = "USERS";
                for (const auto &row : res)
                    out += " " + row[0].as<std::string>();

                sendLine(clientSocket_, out);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Ошибка получения списка пользователей: " << e.what() << std::endl;
                sendLine(clientSocket_, "ERROR_DB");
            }
        }
        else if (cmd == "INBOX")
        {
            handleInbox(username);
        }
        else if (cmd == "MESSAGE")
        {
            std::string to;
            iss >> to;
            std::string text;
            std::getline(iss, text);
            if (!text.empty() && text[0] == ' ')
                text = text.substr(1);
            handleMessage(username, to, text);
        }
        else if (cmd == "HISTORY")
        {
            std::string target;
            iss >> target;
            handleHistory(username, target);
        }
        else
        {
            sendLine(clientSocket_, "UNKNOWN_CMD");
        }
    }

    if (authed)
    {
        int userId = getUserId(username);
        if (userId != -1)
        {
            try
            {
                pqxx::work txn(db_->getConnection());
                txn.exec_params(
                    "UPDATE chat.online_status SET status = 'оффлайн' WHERE user_id = $1",
                    userId);
                txn.commit();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Ошибка обновления статуса: " << e.what() << std::endl;
            }
        }
        Logger::getInstance().log("User logged out: " + username);
    }

    Logger::getInstance().log("Клиент отключён socket=" + std::to_string(clientSocket_));
}
