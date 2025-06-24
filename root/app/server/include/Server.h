#ifndef SERVER_H
#define SERVER_H
#include <memory>
#include "Database.h"
class Server
{
public:
    explicit Server(int port);
    ~Server();
    void start();

private:
    int port;
    int serverSocket;
    std::shared_ptr<Database> db_;
    void initSocket();
};
#endif