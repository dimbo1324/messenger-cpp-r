#include <limits>
#include "Input.h"
#include "Exceptions.h"

namespace ChatApp
{
    namespace InputUtils
    {
        char ReadOption()
        {
            char option;
            if (!(std::cin >> option))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw InputException("Ошибка ввода опции.");
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return option;
        }

        std::string ReadLine()
        {
            std::string line;
            if (!std::getline(std::cin, line))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw InputException("Ошибка чтения строки.");
            }
            return line;
        }
    }
}
