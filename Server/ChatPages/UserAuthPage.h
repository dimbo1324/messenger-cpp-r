#pragma once

#include "IPagesCore.h"

class UserAuthPage final : public IPagesCore
{
private:
    std::string available_commands =
        "Доступные команды:\n"
        "/auth:логин:пароль - авторизоваться;\n"
        "/reg - зарегистрироваться;\n"
        "Введите команду: ";

public:
    UserAuthPage(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient, std::shared_ptr<User> &authorizedUser);
    ~UserAuthPage() = default;

    void run() override;

private:
    bool authCommand();

    bool regCommand();
    bool regPage();
    void offerRegisterOrLogin();
};
