#include <iostream>
#include "src/Chat/Chat.h"
#include "PlatformOSInfo/PlatformOSInfo.h"
#if defined(_WIN32)
#include <windows.h>
#endif
int main()
{
#if defined(_WIN32)
    SetConsoleOutputCP(65001);
#endif
    std::cout << "OS Info: " << getOSInfo() << std::endl;
    std::cout << getProcessInfo() << std::endl;
    try
    {
        ChatApp::Chat messenger("127.0.0.1", 8080);
        messenger.start();
        while (messenger.isChatActive())
        {
            messenger.displayLoginMenu();
            while (messenger.getCurrentUser())
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
