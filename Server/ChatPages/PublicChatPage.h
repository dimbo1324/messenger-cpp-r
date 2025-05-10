#pragma once
#include "IPagesCore.h"

class PublicChatPage final : public IPagesCore
{
private:
    /* data */
public:
    PublicChatPage(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient, std::shared_ptr<User> &authorizedUser);
    ~PublicChatPage() = default;

    void run() override;

private:
    void setHideStatusByID(ullong id, msg::status status);
    void deleteMessageByID(ullong id);
};
