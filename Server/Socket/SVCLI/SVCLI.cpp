#include "SVCLI.h"

void SVCLI::run()
{
    std::string cmd;
    UserInput<std::string, std::string> userInputStr;

    do
    {
        Misc::printMessage("Введите команду /log: ", false);
        cmd = userInputStr.getStringIO();
        if (cmd == "/log")
        {
            char buff[CMD_BUFFER];
            Logger logger(buff);
            logger.read();
        }

    } while (1);
}