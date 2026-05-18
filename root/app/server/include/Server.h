#ifndef SERVER_H
#define SERVER_H

#include "Config.h"
#include "Database.h"
#include "SessionRegistry.h"
#include "tcp/SocketOps.h"

#include <atomic>
#include <memory>

class Server
{
public:
    explicit Server(ServerConfig config);
    ~Server();
    void start();

private:
    void initSocket();

    ServerConfig config_;
    tcp::SocketHandle serverSocket_{tcp::kInvalidSocket};
    std::shared_ptr<Database> db_;
    std::shared_ptr<SessionRegistry> sessions_;
    std::atomic<int> activeClients_{0};
};

#endif
