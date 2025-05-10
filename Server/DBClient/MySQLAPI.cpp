#include "MySQLAPI.h"

bool MySQLAPI::initialize()
{
    db_errno = 0;
    Misc::printMessage("Сервер БД: " + server + ":" + port);
    Misc::printMessage("База данных: " + dbname);
    Misc::printMessage("Пользователь БД: " + dbuser);
    Misc::printMessage("Кодировка: " + db_character_set);
    Misc::printMessage("Для работы с БД используется MySQLAPI");
    Misc::printMessage("Изменить параметры сервера можно в файле .console_chat/server.ini");
    Misc::printMessage("\nПодключение к серверу БД...");
    // Получаем дескриптор соединения
    mysql_init(&mysql);
    if (&mysql == nullptr)
    {
        // Если дескриптор не получен — выводим сообщение об ошибке
        db_errno = 1;
        Misc::printMessage("Ошибка: не удается создать MySQL-дескриптор");
        return false;
    }
    unsigned int i = 5;
    mysql_options(&mysql, MYSQL_OPT_CONNECT_TIMEOUT, &i);
    // Подключаемся к серверу
    if (!mysql_real_connect(&mysql, server.data(), dbuser.data(), dbpass.data(), dbname.data(), atoi(port.data()), NULL, 0))
    {
        // Если нет возможности установить соединение с БД выводим сообщение об ошибке
        db_errno = 1;
        Misc::printMessage("Ошибка: невозможно подключиться к базе данных ", false);
        Misc::printMessage(mysql_error(&mysql));
        return false;
    }
    else
    {
        // Если соединение успешно установлено выводим фразу — "Success!"
        Misc::printMessage("Успешное подключение к MySQL!");
    }
    mysql_set_character_set(&mysql, db_character_set.data());
    // Смотрим изменилась ли кодировка на нужную, по умолчанию идёт latin1
    Misc::printMessage("Connection characterset: ", false);
    Misc::printMessage(mysql_character_set_name(&mysql));
    Misc::printMessage("server> ", false);
    return true;
}

std::shared_ptr<User> MySQLAPI::getUserByID(const ullong &userID)
{
    db_errno = 0;
    if (userID == 0)
        return nullptr;
    std::shared_ptr<User> user = nullptr;
    std::string query = "SELECT * FROM `users` INNER JOIN hash_tab ON `users`.id = `hash_tab`.uid WHERE `id` = '" + std::to_string(userID) + "' LIMIT 1;";
    uint result = querySelect(query);
    if (result > 0)
    {
        user = fetchUserRow();
    }
    mysql_free_result(res);
    return user;
}

std::shared_ptr<User> MySQLAPI::getUserByLogin(const std::string &userLogin)
{
    db_errno = 0;
    std::shared_ptr<User> user = nullptr;
    std::string query = "SELECT * FROM `users` INNER JOIN hash_tab ON `users`.id = `hash_tab`.uid WHERE `login` LIKE '" + userLogin + "' LIMIT 1;";
    uint result = querySelect(query);
    if (result > 0)
    {
        user = fetchUserRow();
    }
    mysql_free_result(res);
    return user;
}

ullong MySQLAPI::getCount(std::string table, std::string where)
{
    db_errno = 0;
    ullong count = 0;
    std::string query = "SELECT COUNT(*) FROM `" + table + "` WHERE " + where + ";";
    if (querySelect(query) > 0)
    {
        count = std::strtoull(row[0], nullptr, 10);
    }
    mysql_free_result(res);
    return count;
}

std::string MySQLAPI::userList(ullong &start, ullong &per_page, ullong &capacity)
{
    db_errno = 0;
    std::string query;

    ullong count = getCount("users", "`id` != 0");

    Misc::alignPaginator(start, per_page, count);

    query = "SELECT * FROM `users` WHERE `id` != 0 LIMIT " + std::to_string(start - 1) + ", " + std::to_string(start + per_page) + ";";

    capacity = querySelect(query);

    std::string result;
    for (ullong i = 0; i < capacity; i++)
    {
        result += std::to_string(i + start) + ". "; // порядковый номер
        auto user = fetchUserRow(0, false);
        result += user->userData();
        result += "\n";
        row = mysql_fetch_row(res);
    }
    mysql_free_result(res);
    return result;
}

bool MySQLAPI::saveUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy)
{
    db_errno = 0;
    std::string query;

    query = "`login` LIKE '" + user->getLogin() + "' AND `id` != " + std::to_string(user->getID()) + ";";
    uint result = getCount("users", query);

    if (result > 0)
    {
        login_busy = true;
        return false;
    }

    query = "`email` LIKE '" + user->getEmail() + "' AND `id` != " + std::to_string(user->getID()) + ";";
    result = getCount("users", query);
    if (result > 0)
    {
        email_busy = true;
        return false;
    }

    query = "UPDATE `users` SET"
            "`login` = '" +
            user->getLogin() +
            "', `email` = '" +
            user->getEmail() +
            "', `first_name` = '" + user->getFirstName() +
            "', `last_name` = '" + user->getLastName() +
            "', `registered` = '" + std::to_string(user->getRegistered()) +
            "', `status` = '" + std::to_string(user->getStatus()) +
            "', `session_key` = '" + std::to_string(user->getSessionKey()) + "' WHERE `users`.`id` = " + std::to_string(user->getID()) + ";";

    std::string query2 = "UPDATE `hash_tab` SET `hash` = '" + user->getHash() + "', `salt` = '" + user->getSalt() + "' WHERE `hash_tab`.`uid` = " + std::to_string(user->getID()) + ";";

    if (queryUpdate(query) != 0)
    {
        return false;
    }
    if (queryUpdate(query2) != 0)
    {
        return false;
    }

    return true;
}

bool MySQLAPI::saveUser(std::shared_ptr<User> &user)
{
    std::string query = "UPDATE `users` SET"
                        "`login` = '" +
                        user->getLogin() +
                        "', `email` = '" +
                        user->getEmail() +
                        "', `first_name` = '" + user->getFirstName() +
                        "', `last_name` = '" + user->getLastName() +
                        "', `registered` = '" + std::to_string(user->getRegistered()) +
                        "', `status` = '" + std::to_string(user->getStatus()) +
                        "', `session_key` = '" + std::to_string(user->getSessionKey()) + "' WHERE `users`.`id` = " + std::to_string(user->getID()) + ";";

    std::string query2 = "UPDATE `hash_tab` SET `hash` = '" + user->getHash() + "', `salt` = '" + user->getSalt() + "' WHERE `hash_tab`.`uid` = '" + std::to_string(user->getID()) + "';";

    if (queryUpdate(query) != 0)
    {
        return false;
    }
    if (queryUpdate(query2) != 0)
    {
        return false;
    }

    return true;
}

bool MySQLAPI::addUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy)
{
    db_errno = 0;
    std::string query;

    query = "`login` LIKE '" + user->getLogin() + "';";
    uint result = getCount("users", query);

    if (result > 0)
    {
        login_busy = true;
        return false;
    }

    query = "`email` LIKE '" + user->getEmail() + "';";
    result = getCount("users", query);
    if (result > 0)
    {

        email_busy = true;
        return false;
    }

    query = "INSERT INTO `users` (`id`, `login`, `email`, `first_name`, `last_name`, `registered`, `status`, `session_key`) "
            "VALUES (NULL, '" +
            user->getLogin() +
            "', '" +
            user->getEmail() +
            "', '" +
            user->getFirstName() +
            "', '" +
            user->getLastName() +
            "', '" +
            std::to_string(user->getRegistered()) +
            "', '" +
            std::to_string(user->getStatus()) +
            "', '" +
            std::to_string(user->getSessionKey()) +
            "');";
    if (queryUpdate(query) != 0)
    {
        mysql_free_result(res);
        return false;
    }

    // согласно задания хеши паролей должны быть в отдельной таблице с триггером
    // из за этого приходится у нового пользователя получить автоинкрементный id а потом добавлять хеши
    // если бы хеш хранился в таблице пользователя, можно было бы его засунуть в предыдущем запросе.
    query = "SELECT `id` FROM `users` WHERE `login` LIKE '" + user->getLogin() + "';";
    ullong id;
    if (querySelect(query) > 0)
    {
        id = std::strtoull(row[0], nullptr, 10);
        mysql_free_result(res);
    }
    else
    {
        mysql_free_result(res);
        Misc::printMessage("Не удалось получить автоинкрементный ID пользователя");
        Misc::printMessage("server> ", false);
        return false;
    }

    query = "UPDATE `hash_tab` SET `hash` = '" + user->getHash() + "', `salt` = '" + user->getSalt() + "' WHERE `hash_tab`.`uid` = " + std::to_string(id) + ";";

    if (queryUpdate(query) != 0)
    {
        return false;
    }
    user = getUserByID(id);
    return user != nullptr && db_errno == 0;
}

std::shared_ptr<Message> MySQLAPI::getMessageByID(const ullong &messageID)
{

    return std::shared_ptr<Message>();
}

bool MySQLAPI::addMessage(std::shared_ptr<Message> &message)
{
    db_errno = 0;
    std::string query =
        message->isPrivate()
            ? "INSERT INTO `private_messages` (`id`, `author_id`, `recipient_id`, `text`, `published`, `status`) "
              "VALUES (NULL, '" +
                  std::to_string(message->getAuthorID()) + "', '" +
                  std::to_string(message->getRecipientID()) + "', '" +
                  message->getText() + "', '" +
                  std::to_string(message->getPublished()) + "', '" +
                  std::to_string(message->getStatusInt() + msg::message_delivered) + "');"
            : "INSERT INTO `pub_messages` (`id`, `author_id`, `text`, `published`, `status`) VALUES (NULL, '" +
                  std::to_string(message->getAuthorID()) + "', '" +
                  message->getText() + "', '" +
                  std::to_string(message->getPublished()) + "', '" +
                  std::to_string(message->getStatusInt() + msg::message_delivered) + "');";
    if (queryUpdate(query) != 0)
    {
        return false;
    }
    return db_errno == 0;
}

std::string MySQLAPI::messageList(ullong &reader_id, ullong &start, ullong &per_page, ullong &capacity)
{
    db_errno = 0;
    std::string query;
    std::string queryUpd = "UPDATE `pub_messages` SET `status` = (`status`| " + std::to_string(msg::message_read) + ") & ~ " +
                           std::to_string(msg::message_delivered) +
                           " WHERE (`author_id` != " + std::to_string(reader_id) + ") AND (";

    ullong count = getCount("pub_messages", "1");

    Misc::alignPaginator(start, per_page, count);

    query = "SELECT * FROM `pub_messages` INNER JOIN `users` ON `pub_messages`.`author_id` = `users`.`id` ORDER BY `pub_messages`.`published` ASC LIMIT " + std::to_string(start - 1) + "," + std::to_string(per_page) + ";";

    capacity = querySelect(query);

    std::string result;
    for (ullong i = 0; i < capacity; i++)
    {
        result += std::to_string(i + start) + ". Сообщение\n"; // порядковый номер
        auto message = fetchMessageRow(0, true);
        auto user = fetchUserRow(5, false);

        result += user->userData() + "\n";
        result += message->messageData();
        result += "\n\n";
        row = mysql_fetch_row(res);
        queryUpd += "`id` = " + std::to_string(message->getID());
        if (i + 1 == capacity)
            queryUpd += ");";
        else
            queryUpd += " OR ";
    }
    if (capacity > 0)
        queryUpdate(queryUpd);
    mysql_free_result(res);
    return result;
}

std::string MySQLAPI::messageList(ullong &reader_id, ullong interlocutor_id, ullong &start, ullong &per_page, ullong &capacity)
{
    db_errno = 0;
    std::string query;
    std::string queryUpd = "UPDATE `private_messages` SET `status` = (`status`| " + std::to_string(msg::message_read) + ") & ~ " +
                           std::to_string(msg::message_delivered) +
                           " WHERE (`author_id` != " + std::to_string(reader_id) + ") AND (";

    ullong count = getCount("private_messages", "1");

    Misc::alignPaginator(start, per_page, count);

    query = "SELECT * FROM `private_messages` INNER JOIN `users` ON `private_messages`.`author_id` = `users`.`id` "
            "WHERE (`private_messages`.`author_id` = " +
            std::to_string(reader_id) +
            " AND `private_messages`.`recipient_id` = " +
            std::to_string(interlocutor_id) +
            ") "
            "OR (`private_messages`.`author_id` = " +
            std::to_string(interlocutor_id) +
            " AND `private_messages`.`recipient_id` = " +
            std::to_string(reader_id) +
            ") "
            "ORDER BY `private_messages`.`published` ASC LIMIT " +
            std::to_string(start - 1) + "," + std::to_string(per_page) + ";";

    capacity = querySelect(query);

    std::string result;
    for (ullong i = 0; i < capacity; i++)
    {
        result += std::to_string(i + start) + ". Сообщение\n"; // порядковый номер
        auto message = fetchMessageRow(0, false);
        auto user = fetchUserRow(6, false);

        result += user->userData() + "\n";
        result += message->messageData();
        result += "\n\n";
        row = mysql_fetch_row(res);
        queryUpd += "`id` = " + std::to_string(message->getID());
        if (i + 1 == capacity)
        {
            queryUpd += ");";
            queryUpdate(queryUpd);
        }
        else
            queryUpd += " OR ";
    }
    if (capacity > 0)
        queryUpdate(queryUpd);
    mysql_free_result(res);
    return result;
}

bool MySQLAPI::deleteByID(ullong &id, std::string &&table)
{
    std::string query = "DELETE FROM `" + table + "` WHERE `id` = " + std::to_string(id) + ";";
    queryUpdate(query);
    return db_errno == 0;
}

bool MySQLAPI::setStatus(ullong &id, std::string &&table, uint action, uint new_status)
{
    db_errno = 0;
    std::string set;
    if (action == 0)
        set = "`status` = `status` & ~ '" + std::to_string(new_status) + "'";
    if (action == 1)
        set = "`status` = `status` | '" + std::to_string(new_status) + "'";
    if (action == 3)
        set = "`status` = '" + std::to_string(new_status) + "'";

    std::string query = "UPDATE `" + table + "` SET " + set + " WHERE `id` = " + std::to_string(id) + ";";
    queryUpdate(query);
    return db_errno == 0;
}

void MySQLAPI::DBclose()
{
    mysql_close(&mysql);
}

void MySQLAPI::hello()
{
    Misc::printMessage("This server uses MySQL API");
}

std::shared_ptr<User> MySQLAPI::fetchUserRow(uint startRow, bool getPassData)
{
    if (mysql_num_rows(res))
    {
        std::string hash;
        std::string salt;
        ullong id = std::strtoull(row[startRow], nullptr, 10);
        std::string login = (std::string)row[++startRow];
        std::string email = (std::string)row[++startRow];
        std::string first_name = (std::string)row[++startRow];
        std::string last_name = (std::string)row[++startRow];
        ullong registered = std::strtoull(row[++startRow], nullptr, 10);
        user::status status = (user::status)atoi(row[++startRow]);
        ullong session_key = std::strtoull(row[++startRow], nullptr, 10);
        ++startRow;
        if (getPassData)
        {
            hash = (std::string)row[++startRow];
            salt = (std::string)row[++startRow];
        }
        else
        {
            hash = std::string();
            salt = std::string();
        }
        return std::make_shared<User>(
            id,
            login,
            email,
            first_name,
            last_name,
            registered,
            status,
            session_key,
            hash,
            salt);
    }
    return nullptr;
}

std::shared_ptr<Message> MySQLAPI::fetchMessageRow(uint startRow, bool pub)
{
    if (mysql_num_rows(res))
    {
        ullong id = std::strtoull(row[startRow], nullptr, 10);
        ullong author_id = std::strtoull(row[++startRow], nullptr, 10);
        ullong recipient_id;
        if (!pub)
            recipient_id = std::strtoull(row[++startRow], nullptr, 10);
        std::string text = (std::string)row[++startRow];
        ullong published = std::strtoull(row[++startRow], nullptr, 10);
        msg::status status = (msg::status)atoi(row[++startRow]);
        if (pub)
        {
            auto msg = std::make_shared<Message>(id, author_id, text, published, status);
            return msg;
        }
        else
        {
            auto msg = std::make_shared<Message>(id, author_id, recipient_id, text, published, status);
            return msg;
        }
    }
    return nullptr;
}

uint MySQLAPI::querySelect(std::string &query)
{
    db_errno = 0;
    mysql_query(&mysql, query.data());
    if (res = mysql_store_result(&mysql))
    {
        row = mysql_fetch_row(res);
        return mysql_num_rows(res);
    }
    else
    {
        db_errno = mysql_errno(&mysql);
        Misc::printMessage(mysql_error(&mysql));
        Misc::printMessage("server> ", false);
        return 0;
    }
    return 0;
}

uint MySQLAPI::queryUpdate(std::string &query)
{
    db_errno = 0;
    uint query_result = mysql_query(&mysql, query.data());
    if (query_result != 0)
    {
        db_errno = mysql_errno(&mysql);
        Misc::printMessage(mysql_error(&mysql));
        Misc::printMessage("server> ", false);
        return query_result;
    }
    return query_result;
}
