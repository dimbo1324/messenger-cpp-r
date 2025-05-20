#include "ClientHandler.h"
#include "Logger.h"
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
std::map<std::string, std::string> ClientHandler::credentials_;
std::map<std::string, int> ClientHandler::onlineClients_;

static bool _loadCreds = []()
{
    std::ifstream f("credentials.txt");
    std::string u, p;
    while (f >> u >> p)
    {
        ClientHandler::credentials_[u] = p;
    }
    return true;
}();

ClientHandler::ClientHandler(int sock)
    : clientSocket_(sock)
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
    std::lock_guard<std::mutex> lk(mtx_);
    if (credentials_.count(login))
        return false;
    credentials_[login] = pass;
    std::ofstream f("credentials.txt", std::ios::app);
    f << login << " " << pass << "\n";
    return true;
}

bool ClientHandler::handleLogin(const std::string &login, const std::string &pass)
{
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = credentials_.find(login);
    if (it == credentials_.end() || it->second != pass)
        return false;
    onlineClients_[login] = clientSocket_;
    return true;
}

void ClientHandler::handleInbox(const std::string &login)
{
    std::lock_guard<std::mutex> lk(mtx_);
    std::ifstream in("inbox.txt");
    std::vector<std::pair<std::string, std::string>> msgs;
    std::vector<std::pair<std::string, std::string>> rest;
    std::string line;
    while (std::getline(in, line))
    {
        std::istringstream iss(line);
        std::string to, from, text;
        std::getline(iss, to, '|');
        std::getline(iss, from, '|');
        std::getline(iss, text);
        if (to == login)
            msgs.emplace_back(from, text);
        else
            rest.emplace_back(to, from + "|" + text);
    }
    in.close();
    std::ofstream out("inbox.txt", std::ios::trunc);
    for (auto &r : rest)
    {
        out << r.first << "|" << r.second << "\n";
    }
    out.close();
    sendLine(clientSocket_, "INBOX_COUNT " + std::to_string(msgs.size()));
    for (auto &m : msgs)
    {
        sendLine(clientSocket_, "INBOX_MSG " + m.first + " " + m.second);
    }
    sendLine(clientSocket_, "INBOX_END");
}

void ClientHandler::handleMessage(const std::string &from, const std::string &to, const std::string &text)
{
    {
        std::lock_guard<std::mutex> lk(mtx_);
        std::ofstream f("history.txt", std::ios::app);
        f << from << "|" << to << "|" << text << "\n";
    }
    {
        std::lock_guard<std::mutex> lk(mtx_);
        std::ofstream f("inbox.txt", std::ios::app);
        f << to << "|" << from << "|" << text << "\n";
    }
    {
        std::lock_guard<std::mutex> lk(mtx_);
        if (onlineClients_.count(to))
            sendLine(onlineClients_[to], "MESSAGE " + from + " " + text);
    }
    sendLine(clientSocket_, "MESSAGE_OK");
}

void ClientHandler::handleHistory(const std::string &login, const std::string &target)
{
    std::lock_guard<std::mutex> lk(mtx_);
    std::ifstream in("history.txt");
    std::vector<std::string> hist;
    std::string line;
    while (std::getline(in, line))
    {
        std::istringstream iss(line);
        std::string from, to, text;
        std::getline(iss, from, '|');
        std::getline(iss, to, '|');
        std::getline(iss, text);
        if ((from == login && to == target) || (from == target && to == login))
            hist.push_back("[" + from + " to " + to + "]: " + text);
    }
    in.close();
    for (auto &msg : hist)
        sendLine(clientSocket_, "HIST " + msg);
    sendLine(clientSocket_, "HISTORY_END");
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
            sendLine(clientSocket_, handleRegister(u, p) ? "REGISTER_OK" : "REGISTER_ERROR");
        }
        else if (cmd == "LOGIN")
        {
            std::string u, p;
            iss >> u >> p;
            if (handleLogin(u, p))
            {
                authed = true;
                username = u;
                sendLine(clientSocket_, "LOGIN_OK");
                Logger::getInstance().log("User logged in: " + u);
            }
            else
            {
                sendLine(clientSocket_, "LOGIN_ERROR");
            }
        }
        else if (!authed)
        {
            sendLine(clientSocket_, "ERROR_NOT_AUTH");
        }
        else if (cmd == "LIST")
        {
            std::lock_guard<std::mutex> lk(mtx_);
            std::string out = "USERS";
            for (auto &kv : onlineClients_)
                out += " " + kv.first;
            sendLine(clientSocket_, out);
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
        std::lock_guard<std::mutex> lk(mtx_);
        onlineClients_.erase(username);
        Logger::getInstance().log("User logged out: " + username);
    }
    Logger::getInstance().log("Клиент отключён socket=" + std::to_string(clientSocket_));
}