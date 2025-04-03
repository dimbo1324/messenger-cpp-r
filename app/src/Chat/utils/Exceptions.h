#pragma once
#include <exception>
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
}
