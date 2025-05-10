#include "../Misc/Misc.h"

#if defined(_WIN64) || defined(_WIN32)
#include "Socket/WinServer/WinServer.h"
#include <io.h>
#include <fcntl.h>

#elif defined(__linux__)

#include "Socket/LinuxServer/LinuxServer.h"

#endif

typedef unsigned int uint;

const char config_file[]{".console_chat/server.ini"};

int main(int argc, const char *argv[])
{
#if defined(_WIN64) || defined(_WIN32)
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
#endif

    std::string port = Misc::getConfigValue(".console_chat/server.ini", "GENERAL", "sv_port");
    if (port.empty())
        port = "7777";
    std::string sv_message =
        "SERVER IS LISTENING THROUGH THE PORT: " + port + " WITHIN A LOCAL SYSTEM\n" +
        "Сервисный администратор: логин - admin пароль - 1234\n"
        "Авторизуйтесь в клиенте и смените пароль командой /profile\n"
        "Чтобы завершить работу сервера, авторизуйтесь в клиенте с администраторскими правами и введите команду /sv_quit";
    Misc::printMessage(sv_message);

    server_socket(port.data());
#if defined(_WIN64) || defined(_WIN32)
    system("pause");
#endif

    return 0;
}
