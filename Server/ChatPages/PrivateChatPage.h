#pragma once
#include "IPagesCore.h"
class PrivateChatPage final : public IPagesCore
{
private:
    std::shared_ptr<User> interlocutorUser = nullptr;
    std::string caption = "Страница личных сообщений";
    std::string commands_text =
        "\nКоманда: /m:число - перейти к сообщению №..;"
        "\nКоманда: /pp:число - установить количество сообщений на страницу;"
        "\nКоманда: /pclear - установить режим: всегда последние 20 сообщений;"
        "\nКоманда: /u:userid - начать приватную беседу пользователем userid;"
        "\nКоманда: /ul:login - начать приватную беседу пользователем login;"
        "\nКоманда: /chat - перейти в общий чат;"
        "\nКоманда: /help - информация о других командах;";

public:
    PrivateChatPage(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient, std::shared_ptr<User> &authorizedUser);
    ~PrivateChatPage() = default;
    void run() override;

    void privateChat();
};
