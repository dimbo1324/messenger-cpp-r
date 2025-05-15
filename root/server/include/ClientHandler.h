#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <thread>
#include <string>

class ClientHandler
{
public:
    explicit ClientHandler(int clientSocket);
    ~ClientHandler();

    void handle() = delete;

private:
    int clientSocket;
    std::thread handlerThread;

    void run();
};

#endif
