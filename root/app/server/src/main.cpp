#include "Config.h"
#include "Logger.h"
#include "Server.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#ifndef _WIN32
#include <csignal>
#endif

int main(int argc, char *argv[])
{
#ifndef _WIN32
    std::signal(SIGPIPE, SIG_IGN);
#endif

    try
    {
        ServerConfig config = loadServerConfig();
        if (argc == 2)
        {
            config.port = std::stoi(argv[1]);
        }
        Server server(config);
        server.start();
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("server failed: ") + e.what());
        std::cerr << "Server failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
