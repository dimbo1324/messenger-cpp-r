#include "UserAuthPage.h"

UserAuthPage::UserAuthPage(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient, std::shared_ptr<User> &authorizedUser)
    : IPagesCore(_cmd_buffer, _dbclient, authorizedUser) {}

void UserAuthPage::run()
{
    buffer.setUserInputCount(100);
    // если пришла команда auth
    if (authCommand())
    {
        return;
    }

    if (regCommand())
    {
        return;
    }

    if (regPage())
    {

        return;
    }
    offerRegisterOrLogin();
}

bool UserAuthPage::authCommand()
{
    if (commands[0] == "/auth")
    {
        // если пришла команда /auth и 2 параметра это логин и пароль
        if (commands.size() == 3)
        {
            auto u = dbClient.DBprovider()->getUserByLogin(commands[1]);
            if (u == nullptr)
            {
                // если пользователя нет - отправляем сообщение клиенту.
                data_text = "Пользователь с таким логином не существует.\n" + available_commands;
                buffer.addFlags(sv::get_string, sv::clear_console);
                return true;
            }
            else
            {
                if (u->validatePass(commands[2]))
                {
                    session_key = Misc::getRandomKey();
                    u->setSessionKey(session_key);
                    dbClient.DBprovider()->saveUser(u);

                    buffer.removeFlag(sv::get_string);
                    buffer.addFlags(sv::no_input, sv::clear_console, sv::write_session);
                    buffer.setSessionKey(session_key);
                    buffer.writeDynData(u->getLogin(), "none", "/chat"); // редирект в общий чат
                    data_text = " ";
                    return true;
                }
                else
                {
                    data_text = "Неверный пароль.\n" + available_commands;
                    buffer.addFlags(sv::get_string, sv::clear_console);
                    return true;
                }
            }
        }
    }

    return false;
}

bool UserAuthPage::regCommand()
{
    if (commands[0] == "/reg")
    {
        // ввод логина
        buffer.createFlags(sv::clear_console, sv::get_string);
        buffer.writeDynDataPos("/register", PAGE_TEXT_COUNT);
        data_text = "Регистрация.\n"
                    "Введите логин (100): ";
        return true;
    }
    return false;
}

bool UserAuthPage::regPage()
{
    if (page_parsed[0] != "/register")
        return false;
    uint _count;
    if (page_parsed.size() == 1)
    {
        // введен логин
        _count = dbClient.DBprovider()->getCount("users", "`login` LIKE '" + cmd_text + "' LIMIT 1");
        if (_count > 0)
        {
            buffer.createFlags(sv::clear_console, sv::get_string);
            buffer.writeDynDataPos("none", CMD_TEXT_COUNT);
            data_text = "Регистрация.\n"
                        "Логин занят.\n"
                        "Введите логин (100): ";
            return true;
        }
        // ввод email
        buffer.createFlags(sv::clear_console, sv::get_string);
        buffer.writeDynDataPos(page_text + "\n" + cmd_text, PAGE_TEXT_COUNT);
        buffer.writeDynDataPos("none", CMD_TEXT_COUNT);
        data_text = "Регистрация.\n"
                    "Введите E-mail (100): ";
        return true;
    }
    if (page_parsed.size() == 2)
    {
        // введен емайл
        _count = dbClient.DBprovider()->getCount("users", "`email` LIKE '" + cmd_text + "' LIMIT 1");
        if (_count > 0)
        {
            buffer.createFlags(sv::clear_console, sv::get_string);
            buffer.writeDynDataPos("none", CMD_TEXT_COUNT);
            data_text = "Регистрация.\n"
                        "E-mail занят.\n"
                        "Введите E-mail (100): ";
            return true;
        }
        std::vector<std::string> email_parse = Misc::stringExplode(cmd_text, "@");
        uint symbols = 0;
        for (int i{0}; i < cmd_text.size(); i++)
        {
            if (cmd_text[i] == '@')
                symbols++;
            if (symbols > 1)
                break;
        }
        if (email_parse.size() != 2 || symbols > 1)
        {
            buffer.createFlags(sv::clear_console, sv::get_string);
            buffer.writeDynDataPos("none", CMD_TEXT_COUNT);
            data_text = "Регистрация.\n"
                        "E-mail должен иметь символ @ в середине.\n"
                        "Введите E-mail (100): ";
            return true;
        }

        // ввод имени
        buffer.createFlags(sv::clear_console, sv::get_string);
        buffer.writeDynDataPos(page_text + "\n" + cmd_text, PAGE_TEXT_COUNT); // записываем email в буфер
        buffer.writeDynDataPos("none", CMD_TEXT_COUNT);
        data_text = "Регистрация.\n"
                    "Введите Имя (100): ";
        return true;
    }
    if (page_parsed.size() == 3)
    {
        // введено имя

        // ввод фамилии
        buffer.createFlags(sv::clear_console, sv::get_string);
        buffer.writeDynDataPos(page_text + "\n" + cmd_text, PAGE_TEXT_COUNT);
        buffer.writeDynDataPos("none", CMD_TEXT_COUNT);
        data_text = "Регистрация.\n"
                    "Введите Фамилию (100): ";
        return true;
    }
    if (page_parsed.size() == 4)
    {
        // введена фамилия

        // ввод пароля
        buffer.createFlags(sv::clear_console, sv::get_string);
        buffer.writeDynDataPos(page_text + "\n" + cmd_text, PAGE_TEXT_COUNT);
        buffer.writeDynDataPos("none", CMD_TEXT_COUNT);
        data_text = "Регистрация.\n"
                    "Введите пароль (100): ";

        return true;
    }

    if (page_parsed.size() == 5)
    {
        // всё введено
        auto u = std::make_shared<User>(page_parsed[1], page_parsed[2], page_parsed[3], page_parsed[4], cmd_text);
        bool email_busy = false, login_busy = false;
        bool res = dbClient.DBprovider()->addUser(u, login_busy, email_busy);
        if (login_busy)
        {
            buffer.createFlags(sv::clear_console, sv::get_string);
            data_text = "Во время регистрации логин был занят другим пользователем.\n"
                        "Введите команду /reg чтобы зарегистрироваться снова.\n"
                        "Команда /help - справка."
                        "\nВведите команду: ";
            return true;
        }
        else if (email_busy)
        {
            buffer.createFlags(sv::clear_console, sv::get_string);
            data_text = "Во время регистрации E-mail был занят другим пользователем.\n"
                        "Введите команду /reg чтобы зарегистрироваться снова.\n"
                        "Команда /help - справка."
                        "\nВведите команду: ";
            return true;
        }
        else if (!res)
        {
            buffer.createFlags(sv::clear_console, sv::get_string);
            data_text = "Не удалось завершить регистрацию по техническим причинам на сервере.\n"
                        "Обратитесь к серверному администратору.\n"
                        "Команда /hello - опрос сервера."
                        "\nВведите команду: ";
            return true;
        }

        // пользователь удачно добавлен в базу
        session_key = Misc::getRandomKey();
        u->setSessionKey(session_key);
        dbClient.DBprovider()->saveUser(u);
        buffer.removeFlag(sv::get_string);
        buffer.addFlags(sv::no_input, sv::clear_console, sv::write_session);
        buffer.setSessionKey(session_key);
        buffer.writeDynData(u->getLogin(), "none", "/chat"); // редирект в общий чат
        data_text = " ";
        return true;
    }

    return false;
}

void UserAuthPage::offerRegisterOrLogin()
{
    data_text = "Вы не авторизованы.\n" + available_commands;
    buffer.addFlags(sv::get_string, sv::clear_console);
}
