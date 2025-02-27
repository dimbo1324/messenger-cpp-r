#include <iostream>
#include "src/Chat/Chat.h"

int main()
{
    try
    {

        ChatApp::Chat messenger;
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
    }
    return 0;
}
