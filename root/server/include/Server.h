#ifndef SERVER_H
#define SERVER_H

class Server
{
public:
    explicit Server(int port);
    ~Server();

    void start();

private:
    int port;
    int serverSocket;

    void initSocket();
};

#endif
