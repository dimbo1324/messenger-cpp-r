#include "LinuxClient.h"
#if defined(__linux__)
char data_buffer[DATA_BUFFER];
char cmd_buffer[CMD_BUFFER];
int socket_descriptor;
struct sockaddr_in serveraddress;
int client_socket(char server_address[], char port[])
{
    for (int i{0}; i < CMD_BUFFER; i++)
        cmd_buffer[i] = '\0';
    ClientHandler handler(cmd_buffer);
    handler.Initialise();
    BufferActions buffer(cmd_buffer);
    serveraddress.sin_addr.s_addr = inet_addr(server_address);
    serveraddress.sin_port = htons(atoi(port));
    serveraddress.sin_family = AF_INET;
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (connect(socket_descriptor, (struct sockaddr *)&serveraddress, sizeof(serveraddress)) < 0)
    {
        std::cout << std::endl
                  << " Something went wrong Connection Failed" << std::endl;
        exit(1);
    }
    struct timeval tv;
    tv.tv_sec = 8;
    tv.tv_usec = 0;
    auto in = setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
    while (handler.getWork())
    {
        handler.Run();
        if (!handler.getWork())
            break;
        buffer.addFlags(sv::cmd_buffer);
        auto s = sendto(socket_descriptor, cmd_buffer, CMD_BUFFER, 0, nullptr, sizeof(serveraddress));
        auto result = recvfrom(socket_descriptor, cmd_buffer, sizeof(cmd_buffer), 0, nullptr, nullptr);
        if (result == -1)
        {
            handler.setDataText("Сервер не ответил на ваш запрос. Обратитесь к администратору.\nВведите команду /hello чтобы опросить сервер: ");
            buffer.createFlags(sv::get_string);
            auto login = buffer.getDynDataS(LOGIN_COUNT);
            buffer.writeDynData(login, "none", "none");
            continue;
        }
        if (!buffer.hasFlag(sv::cmd_buffer))
        {
            Misc::printMessage("Client: от сервера пришли поврежденные данные командного буфера. Сессия будет сброшена.");
            handler.Initialise();
            continue;
        }
        buffer.removeFlag(sv::cmd_buffer);
        std::string data_text;
        for (unsigned char i{0}; i < cmd_buffer[1]; i++)
        {
            recvfrom(socket_descriptor, data_buffer, sizeof(data_buffer), 0, nullptr, nullptr);
            if (data_buffer[0] != sv::data_buffer)
            {
                Misc::printMessage("Client: от сервера пришли поврежденные текстовые данные. Сессия будет сброшена.");
                handler.Initialise();
                continue;
            }
            if (Misc::getInt(data_buffer, 1) > DATA_BUFFER - 5)
            {
                Misc::printMessage("Client: от сервера пришли поврежденные текстовые данные неверной длины. Сессия будет сброшена.");
                handler.Initialise();
                continue;
            }
            data_text += Misc::getString(data_buffer, 1);
        }
        handler.setDataText(data_text);
    }
    close(socket_descriptor);
    return 0;
}
#endif