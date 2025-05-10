#include "UserProfilePage.h"

UserProfilePage::UserProfilePage(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient, std::shared_ptr<User> &authorizedUser)
    : IPagesCore(_cmd_buffer, _dbclient, authorizedUser) {}

void UserProfilePage::run()
{
    buffer.setUserInputCount(107);
    profileCommands();

    buffer.writeDynData(login, "/profile", cmd_text);
    data_text = "Страница редактирования профиля.\nМаксимальное количество символов - 100";
    data_text += AuthorizedUser->userData();

    data_text += service_message;
    data_text += "\nКоманда: /login:new - изменить логин на new;"
                 "\nКоманда: /email:new - изменить E-mail на new;"
                 "\nКоманда: /fname:new - изменить имя на new;"
                 "\nКоманда: /lname:new - изменить фамилию на new;"
                 "\nКоманда: /pass:new - изменить пароль на new"
                 "\nКоманда: /chat - перейти в общий чат;"
                 "\nКоманда: /help - информация по другим командам;"
                 "\nВведите команду: ";

    buffer.createFlags(sv::clear_console, sv::get_string);
}

void UserProfilePage::profileCommands()
{

    if (commands.size() == 2 && profile_cmds.contains(commands[0]))
    {
        bool login_busy = false;
        bool email_busy = false;
        std::string new_value = commands[1];
        if (commands[0] == "/login")
        {
            if (AuthorizedUser->getLogin() == new_value)
            {
                service_message += "\n\nВы уже используете этот логин\n";
                return;
            }
            AuthorizedUser->setLogin(new_value);

            dbClient.DBprovider()->saveUser(AuthorizedUser, login_busy, email_busy);

            if (login_busy)
            {
                service_message += "\n\nЛогин занят\n";
            }
            else
            {
                login = new_value;
                buffer.writeDynData(login, "/profile", cmd_text);
                service_message += "\n\nЛогин изменен\n";
            }
            return;
        }
        if (commands[0] == "/email")
        {
            if (AuthorizedUser->getEmail() == new_value)
            {
                service_message += "\n\nВы уже используете этот E-mail\n";
                return;
            }
            AuthorizedUser->setEmail(new_value);

            dbClient.DBprovider()->saveUser(AuthorizedUser, login_busy, email_busy);

            if (email_busy)
            {
                service_message += "\nE-mail занят\n";
            }
            return;
        }
        if (commands[0] == "/fname")
        {
            if (AuthorizedUser->getFirstName() == new_value)
            {
                service_message += "\n\nВы уже используете это имя\n";
                return;
            }
            AuthorizedUser->setFirstName(new_value);
            dbClient.DBprovider()->saveUser(AuthorizedUser, login_busy, email_busy);
            service_message += "\nИмя изменено\n";
            return;
        }
        if (commands[0] == "/lname")
        {
            if (AuthorizedUser->getLastName() == new_value)
            {
                service_message += "\n\nВы уже используете эту фамилию\n";
                return;
            }
            AuthorizedUser->setLastName(new_value);
            dbClient.DBprovider()->saveUser(AuthorizedUser, login_busy, email_busy);
            service_message += "\nФамилия изменена\n";
            return;
        }
        if (commands[0] == "/pass")
        {
            auto tmp_pass = new_value; // пароль уничтожается при проверках и создании
            if (AuthorizedUser->validatePass(new_value))
            {
                service_message += "\n\nВы уже используете этот пароль\n";
                return;
            }
            AuthorizedUser->setPass(tmp_pass);
            dbClient.DBprovider()->saveUser(AuthorizedUser, login_busy, email_busy);
            service_message += "\nПароль изменен\n";
            return;
        }
    }
}
