#pragma once
#include <exception>
#include <string>

namespace ChatApp
{
    class UserLoginException : public std::exception
    {
    public:
        const char *what() const noexcept override
        {
            return "Ошибка: логин пользователя занят.";
        }
    };

    class UserNameException : public std::exception
    {
    public:
        const char *what() const noexcept override
        {
            return "Ошибка: имя пользователя занято.";
        }
    };

    class InputException : public std::exception
    {
    private:
        std::string message;
    public:
        InputException(const std::string &msg) : message(msg) {}
        const char *what() const noexcept override
        {
            return message.c_str();
        }
    };

    class TimeZoneException : public std::exception
    {
    public:
        const char *what() const noexcept override
        {
            return "Ошибка: не удалось установить часовой пояс.";
        }
    };
}
