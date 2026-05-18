#include "ClientHandler.h"
#include "Logger.h"

#include <algorithm>
#include <sstream>
#include <utility>
#include <vector>

namespace
{
    std::string statusToText(DbStatus status)
    {
        switch (status)
        {
        case DbStatus::Ok:
            return "OK";
        case DbStatus::ValidationError:
            return "VALIDATION";
        case DbStatus::Unauthorized:
            return "UNAUTHORIZED";
        case DbStatus::Forbidden:
            return "FORBIDDEN";
        case DbStatus::NotFound:
            return "NOT_FOUND";
        case DbStatus::Duplicate:
            return "DUPLICATE";
        case DbStatus::Banned:
            return "BANNED";
        case DbStatus::Error:
            return "DB";
        }
        return "ERROR";
    }

    std::string tail(std::istringstream &iss)
    {
        std::string value;
        std::getline(iss, value);
        if (!value.empty() && value.front() == ' ')
        {
            value.erase(value.begin());
        }
        return value;
    }

    int parseIntOrDefault(const std::string &value, int fallback)
    {
        if (value.empty())
        {
            return fallback;
        }
        try
        {
            return std::stoi(value);
        }
        catch (...)
        {
            return fallback;
        }
    }
}

ClientHandler::ClientHandler(tcp::SocketHandle clientSocket,
                             std::shared_ptr<Database> db,
                             std::shared_ptr<SessionRegistry> sessions,
                             std::size_t maxFrameSize)
    : clientSocket_(clientSocket),
      db_(std::move(db)),
      sessions_(std::move(sessions)),
      maxFrameSize_(maxFrameSize)
{
}

ClientHandler::~ClientHandler()
{
    cleanupSession();
    tcp::closeSocket(clientSocket_);
    clientSocket_ = tcp::kInvalidSocket;
}

bool ClientHandler::deliverFrame(const std::string &payload)
{
    return sendResponse(payload);
}

void ClientHandler::disconnectFromAdmin()
{
    sendResponse("KICKED");
    tcp::closeSocket(clientSocket_);
    clientSocket_ = tcp::kInvalidSocket;
}

bool ClientHandler::sendResponse(const std::string &payload)
{
    std::lock_guard<std::mutex> lock(sendMutex_);
    return tcp::sendFrame(clientSocket_, payload);
}

bool ClientHandler::requireAuth()
{
    if (!authed_)
    {
        sendResponse("ERROR_NOT_AUTH");
        return false;
    }
    return true;
}

bool ClientHandler::requireModerator()
{
    if (!requireAuth())
    {
        return false;
    }
    if (role_ != "admin" && role_ != "moderator")
    {
        sendResponse("ERROR_FORBIDDEN moderator role required");
        return false;
    }
    return true;
}

bool ClientHandler::handleRegister(const std::string &login, const std::string &pass)
{
    const auto result = db_->registerUser(login, pass);
    if (result.status == DbStatus::Ok)
    {
        sendResponse("REGISTER_OK");
        return true;
    }

    sendResponse("REGISTER_ERROR " + statusToText(result.status) + " " + result.message);
    return false;
}

bool ClientHandler::handleLogin(const std::string &login, const std::string &pass)
{
    const auto result = db_->loginUser(login, pass);
    if (result.status != DbStatus::Ok)
    {
        sendResponse("LOGIN_ERROR " + statusToText(result.status) + " " + result.message);
        return false;
    }

    cleanupSession();
    authed_ = true;
    userId_ = result.userId;
    username_ = result.login;
    role_ = result.role;
    sessions_->addSession(userId_, this);
    sendResponse("LOGIN_OK " + role_);
    return true;
}

void ClientHandler::handleLogout()
{
    if (!authed_)
    {
        sendResponse("LOGOUT_OK");
        return;
    }
    cleanupSession();
    sendResponse("LOGOUT_OK");
}

void ClientHandler::handleList()
{
    if (!requireAuth())
    {
        return;
    }

    sendResponse("USERS_BEGIN");
    for (const auto &user : db_->listUsers())
    {
        sendResponse("USER " + user.login + " " + user.status + " " + user.role + " " + user.lastSeen);
    }
    sendResponse("USERS_END");
}

void ClientHandler::handleInbox()
{
    if (!requireAuth())
    {
        return;
    }

    sendResponse("INBOX_BEGIN");
    for (const auto &msg : db_->getInbox(userId_, 100))
    {
        sendResponse("INBOX_MSG " + msg.sender + " " + msg.createdAt + " " + msg.text);
    }
    sendResponse("INBOX_END");
}

void ClientHandler::handleMessage(const std::string &to, const std::string &text)
{
    if (!requireAuth())
    {
        return;
    }

    const auto result = db_->sendMessage(userId_, to, text);
    if (result.status != DbStatus::Ok)
    {
        sendResponse("MESSAGE_ERROR " + statusToText(result.status) + " " + result.message);
        return;
    }

    sendResponse("MESSAGE_OK " + std::to_string(result.id));
    const auto receiver = db_->getUserByLogin(to);
    if (receiver)
    {
        sessions_->deliverToUser(receiver->id, "MESSAGE " + username_ + " " + text);
    }
}

void ClientHandler::handleHistory(const std::string &target, int limit, int offset)
{
    if (!requireAuth())
    {
        return;
    }

    sendResponse("HISTORY_BEGIN");
    for (const auto &msg : db_->getHistory(userId_, target, limit, offset))
    {
        sendResponse("HIST " + msg.sender + " " + msg.receiver + " " + msg.createdAt + " " + msg.text);
    }
    sendResponse("HISTORY_END");
}

void ClientHandler::handleAllMessages(int limit)
{
    if (!requireModerator())
    {
        return;
    }

    sendResponse("ALL_MESSAGES_BEGIN");
    for (const auto &msg : db_->getAllMessages(limit))
    {
        sendResponse("MSG " + msg.sender + " " + msg.receiver + " " + msg.createdAt + " " + msg.text);
    }
    sendResponse("ALL_MESSAGES_END");
}

void ClientHandler::handleBan(const std::string &login)
{
    if (!requireModerator())
    {
        return;
    }
    const auto result = db_->banUser(userId_, login);
    if (result.status == DbStatus::Ok)
    {
        if (result.id >= 0)
        {
            sessions_->disconnectUser(result.id);
        }
        sendResponse("OK");
        return;
    }
    sendResponse("ERROR_" + statusToText(result.status) + " " + result.message);
}

void ClientHandler::handleUnban(const std::string &login)
{
    if (!requireModerator())
    {
        return;
    }
    const auto result = db_->unbanUser(userId_, login);
    sendResponse(result.status == DbStatus::Ok ? "OK" : "ERROR_" + statusToText(result.status) + " " + result.message);
}

void ClientHandler::handleKick(const std::string &login)
{
    if (!requireModerator())
    {
        return;
    }
    const auto result = db_->kickUser(userId_, login);
    if (result.status != DbStatus::Ok)
    {
        sendResponse("ERROR_" + statusToText(result.status) + " " + result.message);
        return;
    }

    const std::size_t kicked = sessions_->disconnectUser(result.id);
    sendResponse(kicked > 0 ? "OK" : "ERROR_NOT_FOUND active session not found");
}

void ClientHandler::cleanupSession()
{
    if (!authed_)
    {
        return;
    }
    sessions_->removeSession(userId_, this);
    db_->logoutUser(userId_);
    Logger::getInstance().info("user logged out: " + username_);
    authed_ = false;
    userId_ = -1;
    username_.clear();
    role_.clear();
}

void ClientHandler::run()
{
    Logger::getInstance().info("client connected");

    while (true)
    {
        std::string frame;
        const auto status = tcp::receiveFrame(clientSocket_, frame, maxFrameSize_);
        if (status == tcp::ReceiveFrameStatus::Closed)
        {
            break;
        }
        if (status == tcp::ReceiveFrameStatus::TooLarge)
        {
            sendResponse("ERROR_FRAME_TOO_LARGE");
            break;
        }
        if (status != tcp::ReceiveFrameStatus::Ok)
        {
            Logger::getInstance().warn("client frame read failed");
            break;
        }

        std::istringstream iss(frame);
        std::string cmd;
        iss >> cmd;

        if (cmd == "REGISTER")
        {
            std::string login;
            std::string pass;
            iss >> login >> pass;
            handleRegister(login, pass);
        }
        else if (cmd == "LOGIN")
        {
            std::string login;
            std::string pass;
            iss >> login >> pass;
            handleLogin(login, pass);
        }
        else if (cmd == "LOGOUT")
        {
            handleLogout();
        }
        else if (cmd == "LIST")
        {
            handleList();
        }
        else if (cmd == "INBOX")
        {
            handleInbox();
        }
        else if (cmd == "MESSAGE")
        {
            std::string to;
            iss >> to;
            handleMessage(to, tail(iss));
        }
        else if (cmd == "HISTORY")
        {
            std::string target;
            std::string limitText;
            std::string offsetText;
            iss >> target >> limitText >> offsetText;
            handleHistory(target, parseIntOrDefault(limitText, 50), parseIntOrDefault(offsetText, 0));
        }
        else if (cmd == "ALL_MESSAGES")
        {
            std::string limitText;
            iss >> limitText;
            handleAllMessages(parseIntOrDefault(limitText, 100));
        }
        else if (cmd == "BAN")
        {
            std::string login;
            iss >> login;
            handleBan(login);
        }
        else if (cmd == "UNBAN")
        {
            std::string login;
            iss >> login;
            handleUnban(login);
        }
        else if (cmd == "KICK")
        {
            std::string login;
            iss >> login;
            handleKick(login);
        }
        else
        {
            sendResponse("UNKNOWN_CMD");
        }
    }

    cleanupSession();
    Logger::getInstance().info("client disconnected");
}
