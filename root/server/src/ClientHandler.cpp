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
ClientHandler::ClientHandler(int sock, PGconn *dbConn)
    : clientSocket_(sock), dbConn_(dbConn)
{
    handlerThread_ = std::thread(&ClientHandler::run, this);
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
    ::send(sock, l.c_str(), static_cast<int>(l.size()), 0);
}
void ClientHandler::handleRegister(const std::string &login, const std::string &pass)
{
    PGresult *res = PQexecParams(dbConn_,
                                 "INSERT INTO chat.users(login,password) VALUES($1,$2)",
                                 2, nullptr,
                                 (const char *[]){login.c_str(), pass.c_str()},
                                 nullptr, nullptr, 0);
    bool ok = (PQresultStatus(res) == PGRES_COMMAND_OK);
    PQclear(res);
    sendLine(clientSocket_, ok ? "REGISTER_OK" : "REGISTER_ERROR");
}
void ClientHandler::handleLogin(const std::string &login, const std::string &pass)
{
    PGresult *res = PQexecParams(dbConn_,
                                 "SELECT user_id FROM chat.users WHERE login=$1 AND password=$2",
                                 2, nullptr,
                                 (const char *[]){login.c_str(), pass.c_str()},
                                 nullptr, nullptr, 0);
    if (PQntuples(res) == 1)
    {
        authed_ = true;
        username_ = login;
        sendLine(clientSocket_, "LOGIN_OK");
        Logger::getInstance().log("User logged in: " + login);
    }
    else
    {
        sendLine(clientSocket_, "LOGIN_ERROR");
    }
    PQclear(res);
}
void ClientHandler::handleListUsers()
{
    PGresult *res = PQexec(dbConn_,
                           "SELECT login FROM chat.users "
                           "WHERE last_seen > now() - interval '5 minutes'");
    int n = PQntuples(res);
    std::string out = "USERS";
    for (int i = 0; i < n; ++i)
    {
        out += " " + std::string(PQgetvalue(res, i, 0));
    }
    sendLine(clientSocket_, out);
    PQclear(res);
}
void ClientHandler::handleInbox(const std::string &login)
{
    PGresult *res = PQexecParams(dbConn_,
                                 "SELECT u2.login,m.content,m.message_id "
                                 "FROM chat.messages m "
                                 "JOIN chat.users u1 ON u1.user_id=m.recipient_id "
                                 "JOIN chat.users u2 ON u2.user_id=m.sender_id "
                                 "WHERE u1.login=$1 AND m.is_read=FALSE",
                                 1, nullptr,
                                 (const char *[]){login.c_str()},
                                 nullptr, nullptr, 0);
    int n = PQntuples(res);
    sendLine(clientSocket_, "INBOX_COUNT " + std::to_string(n));
    for (int i = 0; i < n; ++i)
    {
        std::string from = PQgetvalue(res, i, 0);
        std::string msg = PQgetvalue(res, i, 1);
        std::string mid = PQgetvalue(res, i, 2);
        sendLine(clientSocket_, "INBOX_MSG " + from + " " + msg);
        // пометить прочитанным
        PGresult *u = PQexecParams(dbConn_,
                                   "UPDATE chat.messages SET is_read=TRUE WHERE message_id=$1",
                                   1, nullptr,
                                   (const char *[]){mid.c_str()},
                                   nullptr, nullptr, 0);
        PQclear(u);
    }
    sendLine(clientSocket_, "INBOX_END");
    PQclear(res);
}
void ClientHandler::handleMessage(const std::string &from,
                                  const std::string &to,
                                  const std::string &text)
{
    PQexecParams(dbConn_,
                 "INSERT INTO chat.messages(sender_id,recipient_id,content) "
                 "VALUES((SELECT user_id FROM chat.users WHERE login=$1),"
                 "(SELECT user_id FROM chat.users WHERE login=$2),$3)",
                 3, nullptr,
                 (const char *[]){from.c_str(), to.c_str(), text.c_str()},
                 nullptr, nullptr, 0);
    sendLine(clientSocket_, "MESSAGE_OK");
}
void ClientHandler::handleHistory(const std::string &a, const std::string &b)
{
    PGresult *res = PQexecParams(dbConn_,
                                 "SELECT u1.login,m.content "
                                 "FROM chat.messages m "
                                 "JOIN chat.users u1 ON u1.user_id=m.sender_id "
                                 "JOIN chat.users u2 ON u2.user_id=m.recipient_id "
                                 "WHERE (u1.login=$1 AND u2.login=$2)"
                                 "   OR (u1.login=$2 AND u2.login=$1)"
                                 "ORDER BY m.sent_at",
                                 2, nullptr,
                                 (const char *[]){a.c_str(), b.c_str()},
                                 nullptr, nullptr, 0);
    for (int i = 0; i < PQntuples(res); ++i)
    {
        std::string from = PQgetvalue(res, i, 0);
        std::string msg = PQgetvalue(res, i, 1);
        sendLine(clientSocket_, "HIST " + from + " " + msg);
    }
    sendLine(clientSocket_, "HISTORY_END");
    PQclear(res);
}
void ClientHandler::run()
{
    char buf[1024];
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
            handleLogin(u, p);
        }
        else if (!authed_)
        {
            sendLine(clientSocket_, "ERROR_NOT_AUTH");
        }
        else if (cmd == "LIST")
        {
            handleListUsers();
        }
        else if (cmd == "INBOX")
        {
            handleInbox(username_);
        }
        else if (cmd == "MESSAGE")
        {
            std::string to;
            iss >> to;
            std::string txt;
            std::getline(iss, txt);
            if (!txt.empty() && txt[0] == ' ')
                txt.erase(0, 1);
            handleMessage(username_, to, txt);
        }
        else if (cmd == "HISTORY")
        {
            std::string other;
            iss >> other;
            handleHistory(username_, other);
        }
        else
        {
            sendLine(clientSocket_, "UNKNOWN_CMD");
        }
    }
    if (authed_)
    {
        Logger::getInstance().log("User logged out: " + username_);
    }
    Logger::getInstance().log("Client disconnected socket=" + std::to_string(clientSocket_));
}
