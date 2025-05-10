#include "LinuxServer.h"
#if defined(__linux__)
char cmd_buffer[CMD_BUFFER];
char data_buffer[DATA_BUFFER];
int socket_file_descriptor, message_size;
socklen_t length;
struct sockaddr_in serveraddress, client;
int server_socket(char port[])
{
    for (int i{0}; i < CMD_BUFFER; i++)
        cmd_buffer[i] = '\0';
    ServerHandler handler(cmd_buffer);
    BufferActions buffer(cmd_buffer);
    Logger logger(cmd_buffer);
    SVCLI svcli;
    std::thread svcli_thread([&svcli]()
                             { svcli.run(); });
    socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddress.sin_port = htons(atoi(port));
    serveraddress.sin_family = AF_INET;
    bind(socket_file_descriptor, (struct sockaddr *)&serveraddress, sizeof(serveraddress));
    while (handler.getWork())
    {
        length = sizeof(client);
        recvfrom(socket_file_descriptor, cmd_buffer, sizeof(cmd_buffer), 0, (struct sockaddr *)&client, &length);
        if (!buffer.hasFlag(sv::cmd_buffer))
        {
            Misc::printMessage("На сервер пришли поврежденные данные");
            continue;
        }
        logger.write();
        buffer.removeFlag(sv::cmd_buffer);
        handler.Run();
        std::string data_text = Misc::ltrimString(handler.getDataText(), (DATA_BUFFER - 5) * 254);
        if (data_text.size() == 0)
            data_text = " ";
        unsigned char data_parts = (data_text.size() / (DATA_BUFFER - 5)) + 1;
        buffer.setDataPacketsCount(data_parts);
        buffer.addFlags(sv::cmd_buffer);
        sendto(socket_file_descriptor, cmd_buffer, sizeof(cmd_buffer), 0, (struct sockaddr *)&client, sizeof(client));
        for (int i = 0; i < data_text.size(); i += DATA_BUFFER - 5)
        {
            data_buffer[0] = sv::data_buffer;
            Misc::writeStringBuffer(data_text.substr(i, DATA_BUFFER - 5), data_buffer, 1);
            sendto(socket_file_descriptor, data_buffer, sizeof(data_buffer), 0, (struct sockaddr *)&client, sizeof(client));
        }
    }
    svcli_thread.detach();
    close(socket_file_descriptor);
    return 0;
}
#endif