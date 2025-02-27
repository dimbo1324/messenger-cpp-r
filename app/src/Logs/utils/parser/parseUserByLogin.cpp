#include <string>
#include "../structUser.h"

User parseUserByLogin(const std::string &line)
{
    User user;
    size_t loginStart = line.find("Login: ") + 7;
    size_t loginEnd = line.find(", Name: ");
    user.login = line.substr(loginStart, loginEnd - loginStart);
    size_t nameStart = loginEnd + 8;
    size_t nameEnd = line.find(", Password: ");
    user.name = line.substr(nameStart, nameEnd - nameStart);
    size_t passStart = nameEnd + 12;
    user.password = line.substr(passStart);
    return user;
}

std::pair<std::string, std::string> parseUserByName(const std::string &line)
{
    size_t nameStart = line.find("Name: ") + 6;
    size_t nameEnd = line.find(", Login: ");
    std::string name = line.substr(nameStart, nameEnd - nameStart);
    size_t loginStart = nameEnd + 9;
    size_t loginEnd = line.find(", Password: ");
    std::string login = line.substr(loginStart, loginEnd - loginStart);
    return {name, login};
}