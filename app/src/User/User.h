#pragma once
#include <string>
namespace ChatApp
{
class User
{
private:
const std::string _login;
std::string _password;
std::string _name;
public:
User(const std::string &login, const std::string &password,
const std::string &name);
const std::string &GetUserLogin() const;
const std::string &GetUserPassword() const;
const std::string &GetUserName() const;
void SetUserPassword(const std::string &password);
void SetUserName(const std::string &name);
};
}
