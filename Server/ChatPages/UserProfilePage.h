#pragma once
#include "IPagesCore.h"
class UserProfilePage final : public IPagesCore
{
private:
    std::set<std::string> profile_cmds{"/login", "/email", "/fname", "/lname", "/pass"};

public:
    UserProfilePage(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient, std::shared_ptr<User> &authorizedUser);
    ~UserProfilePage() = default;
    void run() override;
    void profileCommands();
};
