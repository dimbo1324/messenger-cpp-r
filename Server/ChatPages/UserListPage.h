#pragma once
#include "IPagesCore.h"

class UserListPage final : public IPagesCore
{
private:
    std::set<std::string> admin_cmds{"/ban", "/unban", "/admin", "/unadmin", "/delete"};

public:
    UserListPage(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient, std::shared_ptr<User> &authorizedUser);
    ~UserListPage() = default;

    void run() override;

private:
    void adminCommands();
};
