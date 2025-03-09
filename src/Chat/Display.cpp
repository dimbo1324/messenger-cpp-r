#include "Display.h"
#include <iostream>

namespace ChatApp
{
    namespace DisplayUtils
    {
        void PrintSeparator(const std::string &title)
        {
            if (!title.empty())
            {
                std::cout << "\n--- " << title << " ---\n";
            }
            else
            {
                std::cout << "\n---------------------\n";
            }
        }

        void PrintMessage(const std::string &from, const std::string &to,
                          const std::string &text, const std::string &timestamp)
        {
            std::cout << "Время: " << timestamp << "\n";
            std::cout << "Сообщение от " << from << " для " << to << ":\n"
                      << text << "\n\n";
        }
    }
}
