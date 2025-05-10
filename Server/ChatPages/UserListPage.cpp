#include "UserListPage.h"

UserListPage::UserListPage(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient, std::shared_ptr<User> &authorizedUser)
    : IPagesCore(_cmd_buffer, _dbclient, authorizedUser) {}

void UserListPage::run()
{
    /* если пользователь пришел с другой страницы или ввел команду /pclear */
    if (page_text != "/users" || cmd_text == "/pclear")
    {
        buffer.pgClear();
    }

    /* обработка команд */

    if (commands.size() > 0 && commands[0] != "/users")
    {
        if (cmd_text == "/pclear")
        {
            buffer.pgClear();
        }
        if (cmd_text == "/update")
        {
        }
        if (commands[0] == "/u" && commands.size() == 2)
        {
            uint m = abs(atoi(commands[1].data()));
            m = m == 0 ? 1 : m;
            buffer.setPaginationMode(sv::pagination::message);
            buffer.setPgStart(m);
        }
        if (commands[0] == "/pp" && commands.size() == 2)
        {
            uint pp = abs(atoi(commands[1].data()));
            pp = pp == 0 ? 1 : pp;
            buffer.setPgPerPage(pp);
        }
        if (AuthorizedUser->isAdmin() && commands.size() == 2)
        {
            adminCommands();
        }
    }

    /*Список сообщений*/
    ullong all_users = dbClient.DBprovider()->getCount("users", "`id` != '0'");
    if (all_users < 1)
    {
        data_text += "В чате нет пользователей.\n";
        buffer.createFlags(sv::clear_console, sv::get_string);
        buffer.writeDynData(AuthorizedUser->getLogin(), "/users", "none");
        return;
    }
    ullong start = buffer.getPgStart();
    ullong per_page = buffer.getPgPerPage();
    ullong capacity = 0;

    if (buffer.getPaginationMode() == sv::pagination::last_page)
    {
        start = per_page >= all_users ? 1 : all_users - (per_page - 1);
    }

    ullong lastUser = start + per_page > all_users ? all_users : start + (per_page - 1);
    data_text = dbClient.DBprovider()->userList(start, per_page, capacity);
    if (capacity < 1)
    {
        data_text += "Нет пользователей в выбранном диапазоне.";
    }
    else
    {
        data_text += "\nПоказаны пользователи " + std::to_string(start) + " - " + std::to_string(lastUser) + " из " + std::to_string(all_users);
    }
    data_text += "\nВы: " + AuthorizedUser->userData();
    data_text += service_message;

    data_text += "\nКоманда: /u:число - перейти к пользователю №..;"
                 "\nКоманда: /pp:число - установить количество пользователей на страницу;"
                 "\nКоманда: /pclear - установить режим: всегда последние 20 пользователей;"
                 "\nКоманда: /private:u:userid - начать приватную беседу пользователем userid;"
                 "\nКоманда: /private:ul:login - начать приватную беседу пользователем login;"
                 "\nКоманда: /chat - перейти в общий чат;"
                 "\nКоманда: /help - информация по другим командам;"
                 "\nКоманда: /update - обновить список;";
    if (AuthorizedUser->isAdmin())
    {
        data_text += "\nКоманда: /ban:userid - заблокировать пользователя по userid;";
        data_text += "\nКоманда: /unban:userid - разблокировать пользователя по userid;";
        data_text += "\nКоманда: /delete:userid - удалить пользователя по userid;";
        data_text += "\nКоманда: /admin:userid - дать права администратора по userid;";
        data_text += "\nКоманда: /unadmin:userid - удалить права администратора по userid;";
    }
    data_text += "\nВведите команду: ";
    buffer.createFlags(sv::clear_console, sv::get_string);
    buffer.writeDynData(AuthorizedUser->getLogin(), "/users", "none");
}

void UserListPage::adminCommands()
{
    ullong uid = abs(atoi(commands[1].data()));
    auto user = dbClient.DBprovider()->getUserByID(uid);
    if (user == nullptr && admin_cmds.contains(commands[0]))
    {
        service_message += "\n\nПользователь с userid " + std::to_string(uid) + " не найден\n";
        return;
    }
    if (user->isServiceAdmin())
    {
        service_message += "\n\nНельзя производить модерацию сервисного администратора.\n";
        return;
    }
    if (user->isAdmin() && user->getID() == AuthorizedUser->getID())
    {
        service_message += "\n\nВы не можете модерировать самого себя. Обратитесь к другому администратору.\n";
        return;
    }

    if (commands[0] == "/ban" && commands.size() == 2)
    {
        if (user->isBanned())
        {
            service_message += "\n\nПользователь с userid " + std::to_string(uid) + " уже заблокирован.\n";
            return;
        }
        user->ban();
        dbClient.DBprovider()->saveUser(user);
        service_message += "\n\nПользователь userid " + std::to_string(uid) + " заблокирован.\n";
        return;
    }
    if (commands[0] == "/unban" && commands.size() == 2)
    {
        if (!user->isBanned())
        {
            service_message += "\n\nПользователь с userid " + std::to_string(uid) + " не заблокирован.\n";
            return;
        }
        user->unBan();
        dbClient.DBprovider()->saveUser(user);
        service_message += "\n\nПользователь userid " + std::to_string(uid) + " разблокирован.\n";
        return;
    }
    if (commands[0] == "/delete" && commands.size() == 2)
    {
        dbClient.DBprovider()->deleteByID(uid, "users");
        service_message += "\n\nПользователь userid " + std::to_string(uid) + " удален.\n";
        return;
    }
    if (commands[0] == "/admin" && commands.size() == 2)
    {
        if (user->isAdmin())
        {
            service_message += "\n\nПользователь с userid " + std::to_string(uid) + " уже имеет права администратора.\n";
            return;
        }
        user->toAdmin();
        dbClient.DBprovider()->saveUser(user);
        service_message += "\n\nПользователь userid " + std::to_string(uid) + " получил права администратора.\n";
        return;
    }
    if (commands[0] == "/unadmin" && commands.size() == 2)
    {
        if (!user->isAdmin())
        {
            service_message += "\n\nПользователь с userid " + std::to_string(uid) + " не имеет прав администратора.\n";
            return;
        }
        user->toUser();
        dbClient.DBprovider()->saveUser(user);
        service_message += "\n\nУ пользователя userid " + std::to_string(uid) + " удалены права администратора.\n";
        return;
    }
}
