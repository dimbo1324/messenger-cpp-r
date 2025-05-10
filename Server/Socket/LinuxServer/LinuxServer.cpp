#include "LinuxServer.h"
#if defined(__linux__)
// Номер порта, который будем использовать для приема и передачи

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

    // Создадим UDP сокет
    socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // Зададим порт для соединения
    serveraddress.sin_port = htons(atoi(port));
    // Используем IPv4
    serveraddress.sin_family = AF_INET;
    // Привяжем сокет
    bind(socket_file_descriptor, (struct sockaddr *)&serveraddress, sizeof(serveraddress));
    while (handler.getWork())
    {
        length = sizeof(client);

        recvfrom(socket_file_descriptor, cmd_buffer, sizeof(cmd_buffer), 0, (struct sockaddr *)&client, &length);

        // если пришел не командный буфер - пропускаем.
        if (!buffer.hasFlag(sv::cmd_buffer))
        {
            Misc::printMessage("На сервер пришли поврежденные данные");
            continue;
        }
        logger.write();
        buffer.removeFlag(sv::cmd_buffer);
        handler.Run(); // обработка входящих данных и формирование ответа

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
        sendto(socket_file_descriptor, cmd_buffer, sizeof(cmd_buffer), 0, (struct sockaddr *)&client, sizeof(client));

        // поочередно отправляем пакет данных на клиента
        for (int i = 0; i < data_text.size(); i += DATA_BUFFER - 5)
        {
            // записываем в нулевой байт пакета флаг, что это пакет данных
            data_buffer[0] = sv::data_buffer;
            // записываем строку в пакет
            Misc::writeStringBuffer(data_text.substr(i, DATA_BUFFER - 5), data_buffer, 1);
            // отправляем пакет клиенту, он должен ждать именно пакет данных, а не что-то другое.
            sendto(socket_file_descriptor, data_buffer, sizeof(data_buffer), 0, (struct sockaddr *)&client, sizeof(client));
        }
    }

    // закрываем сокет, завершаем соединение
    svcli_thread.detach();
    close(socket_file_descriptor);
    return 0;
}

#endif