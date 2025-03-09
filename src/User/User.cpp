#include "User.h"

namespace ChatApp
    {
        User::User(const std::string &login, const std::string &password, const std::string &name)
            : _login(login), _password(password), _name(name) {}

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

        void User::SetUserPassword(const std::string &password)
            {
                _password = password;
            }

        void User::SetUserName(const std::string &name)
            {
                _name = name;
            }
    }
