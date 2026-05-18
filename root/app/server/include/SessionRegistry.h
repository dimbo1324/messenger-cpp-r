#ifndef SESSION_REGISTRY_H
#define SESSION_REGISTRY_H

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class ClientHandler;

class SessionRegistry
{
public:
    void addSession(int userId, ClientHandler *handler);
    void removeSession(int userId, ClientHandler *handler);
    std::size_t deliverToUser(int userId, const std::string &payload);
    std::size_t disconnectUser(int userId);

private:
    std::mutex mutex_;
    std::unordered_map<int, std::vector<ClientHandler *>> sessions_;
};

#endif
