#pragma once
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include "../../../Misc/Misc.h"
#include "../../../Misc/BufferActions.h"

#include "ServerChatMap.h"
#include "../../ChatCore/User.h"
#include "../../DBClient/DBClient.h"
#include "../../ChatPages/IPagesCore.h"
#include "../../ChatPages/UserAuthPage.h"
#include "../../ChatPages/PrivateChatPage.h"
#include "../../ChatPages/PublicChatPage.h"
#include "../../ChatPages/UserListPage.h"
#include "../../ChatPages/UserProfilePage.h"

#define DATA_BUFFER 4096 // Размер буфера для данных
#define CMD_BUFFER 4096  // Размер буфера команд и заголовков

typedef unsigned int uint;
typedef unsigned long long ullong;
class ServerHandler
{
private:
    char (&cmd_buffer)[CMD_BUFFER];

    DBClient dbClient;
    std::shared_ptr<User> user;

    bool work = true;

    ServerChatMap chatMap;

    std::set<std::string> pages_set;

    std::string data_buffer_text;

    BufferActions buffer{cmd_buffer};

public:
    explicit ServerHandler(char (&_cmd_buffer)[CMD_BUFFER]);
    ~ServerHandler() = default;
    void Run();
    void badRequest();

    /// @brief Получает статус работы сервера;
    /// @return
    bool getWork();
    /// @brief дает команду завершить работу сервера
    void quit();

    /// @brief Дает команду клиенту очистить консоль
    /// @param status
    void clearConsole(bool status = true);

    /// @brief задает буферу данные по умолчанию
    void clearBuffer();

    /// @brief Возвращает текст который надо отправить клиенту
    /// @return
    std::string &getDataText();

    /// @brief отправляет клиенту сообщение если с БД проблемы.
    void onDBerror();
};