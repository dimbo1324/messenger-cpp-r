#include "PrivateChatPage.h"

PrivateChatPage::PrivateChatPage(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient, std::shared_ptr<User> &authorizedUser)
    : IPagesCore(_cmd_buffer, _dbclient, authorizedUser)
{
}

void PrivateChatPage::run()
{
    buffer.setUserInputCount(1000);
    if (commands.size() > 0 && commands[0] == "/private" && page_text != "/private")
    {
        buffer.writeDynData(AuthorizedUser->getLogin(), "/private", cmd_text);
    }

    /*
    /private:u:userid - если пришел с других страниц
    /u:userid - если уже находится в личных сообщениях
    /private:ul:login
    /ul:login
    */

    ullong interlocutor_id = buffer.getPmUserID();

    //  /private:u:userid
    if (commands.size() == 3 && commands[0] == "/private" && commands[1] == "u")
    {
        interlocutor_id = std::strtoull(commands[2].data(), nullptr, 10);
        buffer.setPmUserID(interlocutor_id);
    }

    //  /u:userid
    if (commands.size() == 2 && page_text == "/private" && commands[0] == "/u")
    {
        interlocutor_id = std::strtoull(commands[1].data(), nullptr, 10);
        buffer.setPmUserID(interlocutor_id);
    }

    // если сработало что из выше
    if (!buffer.PmUserIsNotSelected())
    {
        interlocutorUser = dbClient.DBprovider()->getUserByID(interlocutor_id);
    }

    // /private:ul:login
    if (commands.size() == 3 && commands[0] == "/private" && commands[1] == "ul")
    {
        interlocutorUser = dbClient.DBprovider()->getUserByLogin(commands[2]);
    }
    // /ul:login
    if (commands.size() == 2 && page_text == "/private" && commands[0] == "/ul")
    {
        interlocutorUser = dbClient.DBprovider()->getUserByLogin(commands[1]);
    }

    if (interlocutorUser == nullptr)
    {
        buffer.clearPmUserID();
    }
    else
    {
        interlocutor_id = interlocutorUser->getID();
        buffer.setPmUserID(interlocutor_id);
    }

    if (buffer.PmUserIsNotSelected())
    {
        buffer.createFlags(sv::options::clear_console, sv::options::get_string);
        data_text += caption;
        data_text += "\nПользователь для личных сообщений не найден.";
        data_text += "\nВведите /u:userid - число, userid пользователя с которым хотите начать беседу."
                     "\nВведите /ul:login - логин пользователя с которым хотите начать беседу."
                     "\nКоманда: /users - отобразить список пользователей (можно будет увидеть их userid);"
                     "\nКоманда: /chat - перейти в общий чат;"
                     "\nКоманда: /help - справка по другим командам;"
                     "\nВведите userid или команду: ";
        return;
    }

    privateChat();
}

void PrivateChatPage::privateChat()
{
    /* если пользователь пришел с другой страницы или ввел команду /pclear */
    if (page_text != "/private" || cmd_text == "/pclear")
    {
        buffer.pgClear();
    }

    if (interlocutorUser->isBanned())
    {
        data_text += "\nВы:\t\t" + AuthorizedUser->userData();
        data_text += "\nСобеседник:\t заблокирован. С ним нельзя вести личную беседу.\n";
        data_text += commands_text;
        data_text += "\nВведите команду: ";
        buffer.createFlags(sv::clear_console, sv::get_string);
        buffer.writeDynData(AuthorizedUser->getLogin(), "/private", "none");
        return;
    }

    if (interlocutorUser->getID() == AuthorizedUser->getID())
    {
        data_text += "\nВы: " + AuthorizedUser->userData();
        data_text += "\nНельзя вести личную беседу с самим собой.\n";
        data_text += commands_text;
        data_text += "\nВведите команду: ";
        buffer.createFlags(sv::clear_console, sv::get_string);
        buffer.writeDynData(AuthorizedUser->getLogin(), "/private", "none");
        return;
    }

    /* обработка команд */

    if (commands.size() > 0 && commands[0] != "/private")
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

        // если введена не команда, значит введено сообщение
        if (!is_command)
        {
            std::shared_ptr<Message> msg = std::make_shared<Message>(AuthorizedUser->getID(), interlocutorUser->getID(), cmd_text);
            dbClient.DBprovider()->addMessage(msg);
        }
    }

    /*Список сообщений*/
    ullong reader_id = AuthorizedUser->getID();
    ullong interlocutor_id = interlocutorUser->getID();
    ullong all_messages = dbClient.DBprovider()->getCount("private_messages",
                                                          "(`private_messages`.`author_id` = " +
                                                              std::to_string(reader_id) +
                                                              " AND `private_messages`.`recipient_id` = " +
                                                              std::to_string(interlocutor_id) +
                                                              ") "
                                                              "OR (`private_messages`.`author_id` = " +
                                                              std::to_string(interlocutor_id) +
                                                              " AND `private_messages`.`recipient_id` = " +
                                                              std::to_string(reader_id) +

                                                              ")");
    if (all_messages < 1)
    {
        data_text += "\nВы:\t\t" + AuthorizedUser->userData();
        data_text += "\nСобеседник:\t" + interlocutorUser->userData();
        data_text += "В чате нет сообщений.\nВведите текст первого сообщения: ";
        buffer.createFlags(sv::clear_console, sv::get_string);
        buffer.writeDynData(AuthorizedUser->getLogin(), "/private", "none");
        return;
    }
    ullong start = buffer.getPgStart();
    ullong per_page = buffer.getPgPerPage();
    ullong capacity = 0;

    if (buffer.getPaginationMode() == sv::pagination::last_page)
    {
        start = per_page >= all_messages ? 1 : all_messages - (per_page - 1);
    }

    ullong lastMsg = start + per_page > all_messages ? all_messages : start + (per_page - 1);
    data_text = dbClient.DBprovider()->messageList(reader_id, interlocutor_id, start, per_page, capacity);
    if (capacity < 1)
    {
        data_text += "Нет сообщений в выбранном диапазоне.";
    }
    else
    {
        data_text += "Показаны сообщения " + std::to_string(start) + " - " + std::to_string(lastMsg) + " из " + std::to_string(all_messages);
    }
    data_text += "\nВы:\t\t" + AuthorizedUser->userData();
    data_text += "\nСобеседник:\t" + interlocutorUser->userData();
    data_text += service_message;
    data_text += commands_text;
    data_text += "\nВведите текст сообщения или команду: ";
    buffer.createFlags(sv::clear_console, sv::get_string);
    buffer.writeDynData(AuthorizedUser->getLogin(), "/private", "none");
}
