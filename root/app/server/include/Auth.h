#ifndef AUTH_H
#define AUTH_H

#include <cstddef>
#include <string>

namespace auth
{
    constexpr std::size_t kMinPasswordLength = 10;

    struct PasswordPolicyResult
    {
        bool ok{false};
        std::string message;
    };

    PasswordPolicyResult validatePassword(const std::string &password);
    bool isValidLogin(const std::string &login);
}

#endif
