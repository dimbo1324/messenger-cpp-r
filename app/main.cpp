#include <iostream>
#include "src/Chat/Chat.h"
#include "PlatformOSInfo/PlatformOSInfo.h"

int main()
{
#if defined(_WIN32)
    initConsoleOutput();
#endif
    std::cout << "OS Info: " << getOSInfo() << std::endl;
    std::cout << getProcessInfo() << std::endl;

    try
    {
        ChatApp::Chat messenger("127.0.0.1", 8080);
        messenger.Start();

        while (messenger.ChatIsActive())
        {
            messenger.displayLoginMenu();
            while (messenger.GetCurrentUser())
            {
                messenger.displayUserMenu();
            }
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Ошибка: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
