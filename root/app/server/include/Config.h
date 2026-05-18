#ifndef CONFIG_H
#define CONFIG_H

#include <cstddef>
#include <string>

struct ServerConfig
{
    int port{8080};
    std::string dbConn;
    std::size_t dbPoolSize{4};
    int maxClients{64};
    int readTimeoutSeconds{300};
    std::size_t maxFrameSize{64 * 1024};
    std::string logPath{"logs/user_activity.log"};
    bool tlsEnabled{false};
    std::string tlsCertPath;
    std::string tlsKeyPath;
};

ServerConfig loadServerConfig();

#endif
