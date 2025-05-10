#include "PublicChatPage.h"

PublicChatPage::PublicChatPage(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient, std::shared_ptr<User> &authorizedUser)
    : IPagesCore(_cmd_buffer, _dbclient, authorizedUser)
{
}

void PublicChatPage::run()
{
    buffer.setUserInputCount(1000);
    /* если пользователь пришел с другой страницы или ввел команду /pclear */
    if (page_text != "/chat" || cmd_text == "/pclear")
    {
        buffer.pgClear();
    }

    /* обработка команд */

    if (commands.size() > 0 && cmd_text != "/chat")
    {
        bool is_command = false;
        if (cmd_text == "/pclear")
        {
            buffer.pgClear();
            is_command = true;
        }
        if (cmd_text == "/update")
        {
            is_command = true;
        }
        if (commands[0] == "/m" && commands.size() == 2)
        {
            uint m = abs(atoi(commands[1].data()));
            m = m == 0 ? 1 : m;
            buffer.setPaginationMode(sv::pagination::message);
            buffer.setPgStart(m);
            is_command = true;
        }
        if (commands[0] == "/pp" && commands.size() == 2)
        {
            uint pp = abs(atoi(commands[1].data()));
            pp = pp == 0 ? 1 : pp;
            buffer.setPgPerPage(pp);
            is_command = true;
        }
        if (AuthorizedUser->isAdmin())
        {
            if (commands[0] == "/hide" && commands.size() == 2)
            {
                auto id = std::strtoull(commands[1].data(), nullptr, 10);
                setHideStatusByID(id, msg::status::hidden_);
                is_command = true;
            }
            if (commands[0] == "/unhide" && commands.size() == 2)
            {
                auto id = std::strtoull(commands[1].data(), nullptr, 10);
                setHideStatusByID(id, msg::status::unhide);
                is_command = true;
            }
            if (commands[0] == "/delete" && commands.size() == 2)
            {
                auto id = std::strtoull(commands[1].data(), nullptr, 10);
                deleteMessageByID(id);
                is_command = true;
            }
        }
        // если введена не команда, значит введено сообщение
        if (!is_command)
        {
            std::shared_ptr<Message> msg = std::make_shared<Message>(AuthorizedUser->getID(), cmd_text);
            dbClient.DBprovider()->addMessage(msg);
        }
    }

    /*Список сообщений*/
    ullong all_messages = dbClient.DBprovider()->getCount("pub_messages", "1");
    if (all_messages < 1)
    {
        data_text += "В чате нет сообщений.\nВведите текст первого сообщения: ";
        buffer.createFlags(sv::clear_console, sv::get_string);
        buffer.writeDynData(AuthorizedUser->getLogin(), "/chat", "none");
        return;
    }
    ullong start = buffer.getPgStart();
    ullong per_page = buffer.getPgPerPage();
    ullong capacity = 0;
    ullong reader_id = AuthorizedUser->getID();
    if (buffer.getPaginationMode() == sv::pagination::last_page)
    {
        start = per_page >= all_messages ? 1 : all_messages - (per_page - 1);
    }

    ullong lastMsg = start + per_page > all_messages ? all_messages : start + (per_page - 1);
    data_text = dbClient.DBprovider()->messageList(reader_id, start, per_page, capacity);
    if (capacity < 1)
    {
        data_text += "Нет сообщений в выбранном диапазоне.";
    }
    else
    {
        data_text += "Показаны сообщения " + std::to_string(start) + " - " + std::to_string(lastMsg) + " из " + std::to_string(all_messages);
    }
    data_text += "\nВы: " + AuthorizedUser->userData();
    data_text += service_message;

    data_text += "\nКоманда: /m:число - перейти к сообщению №..;"
                 "\nКоманда: /pp:число - установить количество сообщений на страницу;"
                 "\nКоманда: /pclear - установить режим: всегда последние 20 сообщений;";
    if (AuthorizedUser->isAdmin())
    {
        data_text += "\nКоманда: /hide:число - скрыть сообщение по ID;";
        data_text += "\nКоманда: /unhide:число - открыть сообщение по ID;";
        data_text += "\nКоманда: /delete:число - удалить сообщение по ID;";
    }
    data_text += "\nВведите текст сообщения или команду: ";
    buffer.createFlags(sv::clear_console, sv::get_string);
    buffer.writeDynData(AuthorizedUser->getLogin(), "/chat", "none");
}

void PublicChatPage::setHideStatusByID(ullong id, msg::status status)
{
    if (!AuthorizedUser->isAdmin())
        return;
    uint count_msg = dbClient.DBprovider()->getCount("pub_messages", "`id` = " + std::to_string(id));
    if (count_msg > 0)
    {
        if (status == msg::status::hidden_)
            dbClient.DBprovider()->setStatus(id, "pub_messages", 1, status);
        if (status == msg::status::unhide)
            dbClient.DBprovider()->setStatus(id, "pub_messages", 0, msg::status::hidden_);
    }
    else
    {
        service_message += "\n\nНе найдено сообщение для скрытия.\n";
    }
}

void PublicChatPage::deleteMessageByID(ullong id)
{
    if (!AuthorizedUser->isAdmin())
        return;
    uint count_msg = dbClient.DBprovider()->getCount("pub_messages", "`id` = " + std::to_string(id));
    if (count_msg > 0)
    {
        dbClient.DBprovider()->deleteByID(id, "pub_messages");
    }
    else
    {
        service_message += "\n\nНе найдено сообщение для удаления.\n";
    }
}
