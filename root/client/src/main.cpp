#include "Client.h"
#include <iostream>
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: chat_client <host:port>\n";
        return 1;
    }
    std::string arg = argv[1];
    auto pos = arg.find(':');
    std::string host = arg.substr(0, pos);
    unsigned short port = static_cast<unsigned short>(std::stoi(arg.substr(pos + 1)));
    Client client(host, port);
    client.run();
    return 0;
}
