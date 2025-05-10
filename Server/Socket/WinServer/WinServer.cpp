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

    SOCKET serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Create server socket
    if (INVALID_SOCKET == serSocket)
    {
        std::cout << "socket error!";
        return 0;
    }

    // Set the transmission protocol, port and destination address
    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(atoi(port));
    serAddr.sin_addr.S_un.S_addr = INADDR_ANY;

    if (bind(serSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR) // bind socket to address
    {
        std::cout << "bind error";
        closesocket(serSocket);
        return 0;
    }

    sockaddr_in clientAddr;
    int iAddrlen = sizeof(clientAddr);

    while (handler.getWork())
    {
        // ждем запросы от клиентов
        recvfrom(serSocket, cmd_buffer, CMD_BUFFER, 0, (sockaddr *)&clientAddr, &iAddrlen);

        // если пришел не командный буфер - пропускаем.
        if (!buffer.hasFlag(sv::cmd_buffer))
        {
            Misc::printMessage("На сервер пришли поврежденные данные");
            continue;
        }
        logger.write();
        buffer.removeFlag(sv::cmd_buffer);
        handler.Run();

        // это текстовые данные которые надо отправить на клиент
        // обрезаются спереди если их длина превышает максимальное количество пакетов данных 254 штуки по 4096 байт
        std::string data_text = Misc::ltrimString(handler.getDataText(), (DATA_BUFFER - 5) * 254);

        // если текстовый пакет пуст, добавим в него пробел
        if (data_text.size() == 0)
            data_text = " ";

        // вычисляем количество текстовых пакетов
        unsigned char data_parts = (data_text.size() / (DATA_BUFFER - 5)) + 1;

        // записываем в командный буфер количество текстовых пакетов
        buffer.setDataPacketsCount(data_parts);

        // записываем в командный буфер, что это командный буфер
        buffer.addFlags(sv::cmd_buffer);

        // Отправка пакета команд
        sendto(serSocket, cmd_buffer, CMD_BUFFER, 0, (sockaddr *)&clientAddr, iAddrlen);

        // поочередно отправляем пакет данных на клиента
        for (int i = 0; i < data_text.size(); i += DATA_BUFFER - 5)
        {
            // записываем в нулевой байт пакета флаг, что это пакет данных
            data_buffer[0] = sv::data_buffer;
            // записываем строку в пакет
            Misc::writeStringBuffer(data_text.substr(i, DATA_BUFFER - 5), data_buffer, 1);
            // отправляем пакет клиенту, он должен ждать именно пакет данных, а не что-то другое.
            sendto(serSocket, data_buffer, DATA_BUFFER, 0, (sockaddr *)&clientAddr, iAddrlen);
        }
    }

    // cleanup
    closesocket(serSocket);
    WSACleanup();
    system("pause");
    return 0;
}

#endif