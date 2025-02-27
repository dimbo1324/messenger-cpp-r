#include "./structMessage.h"
#include "./structUser.h"

Message parseMessage(const std::string &line);
User parseUserByLogin(const std::string &line);
std::pair<std::string, std::string> parseUserByName(const std::string &line);