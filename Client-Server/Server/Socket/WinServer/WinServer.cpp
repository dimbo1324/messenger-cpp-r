#include "WinServer.h"
#if defined(_WIN64) || defined(_WIN32)
int server_socket(char port[])
{
    char cmd_buffer[CMD_BUFFER];
    for (int i{0}; i < CMD_BUFFER; i++)
        cmd_buffer[i] = '\0';
    char data_buffer[DATA_BUFFER];
    ServerHandler handler(cmd_buffer);
    BufferActions buffer(cmd_buffer);
    Logger logger(cmd_buffer);
    SVCLI svcli;
    std::thread svcli_thread([&svcli]()
                             { svcli.run(); });
    WSADATA WSAData;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &WSAData) != 0)
        return 0;
    SOCKET serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == serSocket)
    {
        std::cout << "socket error!";
        return 0;
    }
    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(atoi(port));
    serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(serSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
        std::cout << "bind error";
        closesocket(serSocket);
        return 0;
    }
    sockaddr_in clientAddr;
    int iAddrlen = sizeof(clientAddr);
    while (handler.getWork())
    {
        recvfrom(serSocket, cmd_buffer, CMD_BUFFER, 0, (sockaddr *)&clientAddr, &iAddrlen);
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
        sendto(serSocket, cmd_buffer, CMD_BUFFER, 0, (sockaddr *)&clientAddr, iAddrlen);
        for (int i = 0; i < data_text.size(); i += DATA_BUFFER - 5)
        {
            data_buffer[0] = sv::data_buffer;
            Misc::writeStringBuffer(data_text.substr(i, DATA_BUFFER - 5), data_buffer, 1);
            sendto(serSocket, data_buffer, DATA_BUFFER, 0, (sockaddr *)&clientAddr, iAddrlen);
        }
    }
    closesocket(serSocket);
    WSACleanup();
    system("pause");
    return 0;
}
#endif