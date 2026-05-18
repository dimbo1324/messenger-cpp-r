#include "Config.h"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace
{
    std::string trim(const std::string &value)
    {
        const auto begin = value.find_first_not_of(" \t\r\n");
        if (begin == std::string::npos)
        {
            return {};
        }
        const auto end = value.find_last_not_of(" \t\r\n");
        return value.substr(begin, end - begin + 1);
    }

    std::string envOrEmpty(const char *name)
    {
        const char *value = std::getenv(name);
        return value == nullptr ? std::string{} : std::string{value};
    }

    int parseInt(const std::string &name, const std::string &value, int fallback)
    {
        if (value.empty())
        {
            return fallback;
        }
        try
        {
            return std::stoi(value);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error(name + " must be an integer: " + e.what());
        }
    }

    std::size_t parseSize(const std::string &name, const std::string &value, std::size_t fallback)
    {
        if (value.empty())
        {
            return fallback;
        }
        try
        {
            return static_cast<std::size_t>(std::stoul(value));
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error(name + " must be a positive integer: " + e.what());
        }
    }

    std::unordered_map<std::string, std::string> readConfigFile(const std::string &path)
    {
        std::unordered_map<std::string, std::string> result;
        if (path.empty())
        {
            return result;
        }

        std::ifstream in(path);
        if (!in.is_open())
        {
            return result;
        }

        std::string line;
        while (std::getline(in, line))
        {
            const auto comment = line.find('#');
            if (comment != std::string::npos)
            {
                line = line.substr(0, comment);
            }
            const auto eq = line.find('=');
            if (eq == std::string::npos)
            {
                continue;
            }
            result[trim(line.substr(0, eq))] = trim(line.substr(eq + 1));
        }
        return result;
    }

    std::string setting(const std::unordered_map<std::string, std::string> &fileConfig,
                        const char *envName,
                        const char *fileName,
                        const std::string &fallback = {})
    {
        const auto env = envOrEmpty(envName);
        if (!env.empty())
        {
            return env;
        }
        const auto it = fileConfig.find(fileName);
        return it == fileConfig.end() ? fallback : it->second;
    }
}

ServerConfig loadServerConfig()
{
    const std::string configPath = envOrEmpty("MESSENGER_CONFIG");
    auto fileConfig = readConfigFile(configPath.empty() ? "config/server.conf" : configPath);

    ServerConfig config;
    config.dbConn = setting(fileConfig, "CHAT_DB_CONN", "db_conn");
    config.port = parseInt("CHAT_SERVER_PORT", setting(fileConfig, "CHAT_SERVER_PORT", "server_port"), config.port);
    config.dbPoolSize = parseSize("CHAT_DB_POOL_SIZE", setting(fileConfig, "CHAT_DB_POOL_SIZE", "db_pool_size"), config.dbPoolSize);
    config.maxClients = parseInt("CHAT_MAX_CLIENTS", setting(fileConfig, "CHAT_MAX_CLIENTS", "max_clients"), config.maxClients);
    config.readTimeoutSeconds = parseInt("CHAT_READ_TIMEOUT_SEC", setting(fileConfig, "CHAT_READ_TIMEOUT_SEC", "read_timeout_sec"), config.readTimeoutSeconds);
    config.maxFrameSize = parseSize("CHAT_MAX_FRAME_SIZE", setting(fileConfig, "CHAT_MAX_FRAME_SIZE", "max_frame_size"), config.maxFrameSize);
    config.logPath = setting(fileConfig, "CHAT_LOG_PATH", "log_path", config.logPath);
    config.tlsCertPath = setting(fileConfig, "CHAT_TLS_CERT", "tls_cert");
    config.tlsKeyPath = setting(fileConfig, "CHAT_TLS_KEY", "tls_key");
    config.tlsEnabled = setting(fileConfig, "CHAT_ENABLE_TLS", "tls_enabled") == "1";

    if (config.dbConn.empty())
    {
        throw std::runtime_error(
            "CHAT_DB_CONN is not set and config/server.conf was not found or has no db_conn. "
            "Refusing to start with an unsafe default database password.");
    }
    if (config.dbPoolSize == 0)
    {
        throw std::runtime_error("CHAT_DB_POOL_SIZE must be greater than zero");
    }
    if (config.port <= 0 || config.port > 65535)
    {
        throw std::runtime_error("CHAT_SERVER_PORT must be in range 1-65535");
    }
    if (config.maxClients <= 0)
    {
        throw std::runtime_error("CHAT_MAX_CLIENTS must be greater than zero");
    }
    if (config.readTimeoutSeconds <= 0)
    {
        throw std::runtime_error("CHAT_READ_TIMEOUT_SEC must be greater than zero");
    }
    if (config.maxFrameSize < 1024)
    {
        throw std::runtime_error("CHAT_MAX_FRAME_SIZE must be at least 1024 bytes");
    }

    return config;
}
