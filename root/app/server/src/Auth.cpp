#include "Auth.h"

#include <algorithm>
#include <cctype>

namespace auth
{
    PasswordPolicyResult validatePassword(const std::string &password)
    {
        if (password.size() < kMinPasswordLength)
        {
            return {false, "password must be at least 10 characters"};
        }

        bool hasLetter = false;
        bool hasDigit = false;
        for (unsigned char c : password)
        {
            hasLetter = hasLetter || std::isalpha(c);
            hasDigit = hasDigit || std::isdigit(c);
        }

        if (!hasLetter || !hasDigit)
        {
            return {false, "password must contain letters and digits"};
        }
        return {true, {}};
    }

    bool isValidLogin(const std::string &login)
    {
        if (login.size() < 3 || login.size() > 50)
        {
            return false;
        }

        return std::all_of(login.begin(), login.end(), [](unsigned char c) {
            return std::isalnum(c) || c == '_' || c == '-' || c == '.';
        });
    }
}
