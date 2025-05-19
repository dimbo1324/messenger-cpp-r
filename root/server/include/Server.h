#pragma once
#include <libpq-fe.h>
#include <string>
#include "ClientHandler.h"
class Server
{
public:
    explicit Server(int port);
    ~Server();
    void start();

private:
    void initDatabase();
    void initSocket();
    int port;
    int serverSocket;
    PGconn *dbConn_;
};
