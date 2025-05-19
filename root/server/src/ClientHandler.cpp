#include "ClientHandler.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <cstring>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#endif

std::map<std::string, std::string> ClientHandler::credentials_;
std::map<std::string, int> ClientHandler::onlineClients_;
std::map<std::string, std::vector<std::string>> ClientHandler::history_[2];
std::mutex ClientHandler::mtx_;

static bool credsLoaded = []()
{
    std::ifstream f("credentials.txt");
    std::string u, p;
    while (f >> u >> p)
        ClientHandler::credentials_[u] = p;
    return true;
}();

ClientHandler::ClientHandler(int sock)
    : clientSocket_(sock),
      handlerThread_(&ClientHandler::run, this)
{
    handlerThread_.detach();
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
    ::send(sock, l.c_str(), (int)l.size(), 0);
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

void ClientHandler::run()
{
    char buf[1024];
    std::string username;
    bool authed = false;
    Logger::getInstance().log("Подключился socket=" + std::to_string(clientSocket_));

    while (true)
    {
        int n = recv(clientSocket_, buf, sizeof(buf) - 1, 0);
        if (n <= 0)
            break;
        buf[n] = 0;
        std::string line(buf);
        if (line.back() == '\n')
            line.pop_back();

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "REGISTER")
        {
            std::string u, p;
            iss >> u >> p;
            if (handleRegister(u, p))
                sendLine(clientSocket_, "REGISTER_OK");
            else
                sendLine(clientSocket_, "REGISTER_ERROR");
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
                sendLine(clientSocket_, "LOGIN_ERROR");
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
        else if (cmd == "MESSAGE")
        {
            std::string to;
            iss >> to;
            std::string msg;
            std::getline(iss, msg);
            std::lock_guard<std::mutex> lk(mtx_);
            history_[0][username + "->" + to].push_back(msg);
            history_[1][to + "->" + username].push_back(msg);
            if (onlineClients_.count(to))
            {
                int s = onlineClients_[to];
                sendLine(s, "MESSAGE " + username + msg);
            }
            sendLine(clientSocket_, "MESSAGE_OK");
        }
        else if (cmd == "HISTORY")
        {
            std::string to;
            iss >> to;
            std::lock_guard<std::mutex> lk(mtx_);
            auto &hist = history_[0][username + "->" + to];
            for (auto &m : hist)
                sendLine(clientSocket_, "HIST " + m);
            sendLine(clientSocket_, "HISTORY_END");
        }
        else if (cmd == "QUIT")
        {
            break;
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
    }
    Logger::getInstance().log("Отключился socket=" + std::to_string(clientSocket_));
}
