#include "WinClient.h"
#if defined(_WIN64) || defined(_WIN32)

int client_socket(char server_address[], char port[])
{
    char data_buffer[DATA_BUFFER];
    char cmd_buffer[CMD_BUFFER];
    for (int i{0}; i < CMD_BUFFER; i++)
        cmd_buffer[i] = '\0';
    ClientHandler handler(cmd_buffer);
    handler.Initialise();
    BufferActions buffer(cmd_buffer);
    SOCKET socket_descriptor;
    struct sockaddr_in serveraddress;

    WSADATA WSAData;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &WSAData) != 0)
        return 0;

    // Укажем адрес сервера
    serveraddress.sin_addr.s_addr = inet_addr(server_address);
    // Зададим номер порта для соединения с сервером
    serveraddress.sin_port = htons(atoi(port));
    // Используем IPv4
    serveraddress.sin_family = AF_INET;
    // Создадим сокет
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    // Установим соединение с сервером
    if (connect(socket_descriptor, (struct sockaddr *)&serveraddress, sizeof(serveraddress)) < 0)
    {
        std::cout << std::endl
                  << " Something went wrong Connection Failed" << std::endl;
        exit(1);
    }
    int iTimeout = 8000;
    auto iRet = setsockopt(socket_descriptor,
                           SOL_SOCKET,
                           SO_RCVTIMEO,
                           (const char *)&iTimeout,
                           sizeof(iTimeout));
    while (handler.getWork())
    {
        handler.Run();
        if (!handler.getWork())
            break;

        // добавляем в буфер флаг, что это командный буфер
        buffer.addFlags(sv::cmd_buffer);
        // отправка команды на сервер
        auto s = sendto(socket_descriptor, cmd_buffer, CMD_BUFFER, 0, nullptr, sizeof(serveraddress));

        // получение команды от сервера
        auto result = recvfrom(socket_descriptor, cmd_buffer, sizeof(cmd_buffer), 0, nullptr, nullptr);

        // запишем в буфер текст который отобразится если сервер отвалится
        if (result == -1)
        {
            handler.setDataText("Сервер не ответил на ваш запрос. Обратитесь к администратору.\nВведите команду /hello чтобы опросить сервер: ");
            buffer.createFlags(sv::get_string);
            auto login = buffer.getDynDataS(LOGIN_COUNT);
            buffer.writeDynData(login, "none", "none");
            continue;
        }
        // убеждаемся что это был командный буфер
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
            // получение данных с сервера
            recvfrom(socket_descriptor, data_buffer, sizeof(data_buffer), 0, nullptr, nullptr);

            // убеждаемся что это буфер данных
            if (data_buffer[0] != sv::data_buffer)
            {
                Misc::printMessage("Client: от сервера пришли поврежденные текстовые данные. Сессия будет сброшена.");
                handler.Initialise();
                continue;
            }
            // убедимся что размер данных не превышает длину буфера
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

    // cleanup
    closesocket(socket_descriptor);
    WSACleanup();
    system("pause");
    return 0;
}
#endif