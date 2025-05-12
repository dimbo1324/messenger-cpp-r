#include "Client.h"
#include <iostream>
#include <string>
#include <stdexcept>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Использование: chat_client <хост:порт>\n";
        return 1;
    }
    std::string arg = argv[1];
    size_t pos = arg.find(':');
    if (pos == std::string::npos)
    {
        std::cerr << "Ошибка: Неверный формат аргумента. Используйте <хост:порт>.\n";
        return 1;
    }
    std::string host = arg.substr(0, pos);
    std::string port_str = arg.substr(pos + 1);
    if (host.empty())
    {
        std::cerr << "Ошибка: Имя хоста не может быть пустым.\n";
        return 1;
    }
    if (port_str.empty())
    {
        std::cerr << "Ошибка: Порт не может быть пустым.\n";
        return 1;
    }
    unsigned short port = 0;
    try
    {
        int p_int = std::stoi(port_str);
        if (p_int <= 0 || p_int > 65535)
        {
            std::cerr << "Ошибка: Неверный номер порта: " << p_int << ". Порт должен быть в диапазоне 1-65535.\n";
            return 1;
        }
        port = static_cast<unsigned short>(p_int);
    }
    catch (const std::invalid_argument &ia)
    {
        std::cerr << "Ошибка: Неверный формат номера порта (не число): " << port_str << "\n";
        return 1;
    }
    catch (const std::out_of_range &oor)
    {
        std::cerr << "Ошибка: Номер порта вне допустимого диапазона: " << port_str << "\n";
        return 1;
    }
    try
    {
        Client client(host, port);
        client.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Непредвиденная ошибка клиента: " << e.what() << std::endl;
        return 2;
    }
    catch (...)
    {
        std::cerr << "Непредвиденная неизвестная ошибка клиента." << std::endl;
        return 2;
    }
    return 0;
}