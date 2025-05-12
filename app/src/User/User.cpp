#include "User.h"

namespace ChatApp
{
    User::User(const std::string &login, const std::string &password, const std::string &name, int id)
        : _login(login), _password(password), _name(name), _id(id)
    {
    }

    const std::string &User::GetUserLogin() const
    {
        return _login;
    }

    const std::string &User::GetUserPassword() const
    {
        return _password;
    }

    const std::string &User::GetUserName() const
    {
        return _name;
    }

    int User::getId() const
    {
        return _id;
    }

    void User::setId(int id)
    {
        _id = id;
    }

    void User::SetUserPassword(const std::string &password)
    {
        _password = password;
    }

    void User::SetUserName(const std::string &name)
    {
        _name = name;
    }
}
