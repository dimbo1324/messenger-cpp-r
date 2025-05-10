#include <iostream>
#include "../Misc/Misc.h"
#if defined(_WIN64) || defined(_WIN32)

#include <iostream>
#include <string>
#include <io.h>
#include <fcntl.h>

#include "WinClient\WinClient.h"
// команда ОС для очистки консоли
const char clear[]{"cls"};
#elif defined(__linux__)

#include "LinuxClient/LinuxClient.h"
// команда ОС для очистки консоли
const char clear[]{"clear"};

#endif

int main(int argc, const char **argv)
{
#if defined(_WIN64) || defined(_WIN32)

    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
#endif
    /**
     * Адрес и порт сервера указаны в файле .console_chat/client.ini
     * При отсутствии файла, по умолчанию 127.0.0.1:7777
     */
    std::string port = Misc::getConfigValue(".console_chat/client.ini", "GENERAL", "sv_port");
    std::string server_address = Misc::getConfigValue(".console_chat/client.ini", "GENERAL", "sv_address");

    if (port.empty())
        port = "7777";
    if (server_address.empty())
        server_address = "127.0.0.1";

    Misc::printMessage("Клиент подключен к серверу: " + server_address + ":" + port + " Другие параметры подключения можно задать в файле .console_chat/client.ini");
    client_socket(server_address.data(), port.data());
    return 0;
}