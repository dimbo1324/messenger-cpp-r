#include "Server.h"
#include <iostream>
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Использование: chat_server <хост> <порт>\n";
        return 1;
    }
    std::string host = argv[1];
    unsigned short port = static_cast<unsigned short>(std::stoi(argv[2]));
    try
    {
        server::Server srv(host, port);
        srv.run();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Ошибка сервера: " << ex.what() << "\n";
        return 2;
    }
    return 0;
}
