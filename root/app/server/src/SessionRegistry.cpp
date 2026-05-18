#include "SessionRegistry.h"
#include "ClientHandler.h"

#include <algorithm>

void SessionRegistry::addSession(int userId, ClientHandler *handler)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[userId].push_back(handler);
}

void SessionRegistry::removeSession(int userId, ClientHandler *handler)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(userId);
    if (it == sessions_.end())
    {
        return;
    }

    auto &handlers = it->second;
    handlers.erase(std::remove(handlers.begin(), handlers.end(), handler), handlers.end());
    if (handlers.empty())
    {
        sessions_.erase(it);
    }
}

std::size_t SessionRegistry::deliverToUser(int userId, const std::string &payload)
{
    std::vector<ClientHandler *> handlers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = sessions_.find(userId);
        if (it == sessions_.end())
        {
            return 0;
        }
        handlers = it->second;
    }

    std::size_t delivered = 0;
    for (ClientHandler *handler : handlers)
    {
        if (handler != nullptr && handler->deliverFrame(payload))
        {
            ++delivered;
        }
    }
    return delivered;
}

std::size_t SessionRegistry::disconnectUser(int userId)
{
    std::vector<ClientHandler *> handlers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = sessions_.find(userId);
        if (it == sessions_.end())
        {
            return 0;
        }
        handlers = it->second;
    }

    for (ClientHandler *handler : handlers)
    {
        if (handler != nullptr)
        {
            handler->disconnectFromAdmin();
        }
    }
    return handlers.size();
}
