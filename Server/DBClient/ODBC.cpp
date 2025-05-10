#include "ODBC.h"
#if defined(_WIN64) || defined(_WIN32)
bool ODBC::initialize()
{
    connect_data = "DRIVER={MySQL ODBC 8.0 ANSI Driver};SERVER=" + server +
                   ";PORT=" + port +
                   ";DATABASE=" + dbname +
                   ";UID=" + dbuser +
                   ";PWD=" + dbpass + ";";
    Misc::printMessage("Сервер БД: " + server + ":" + port);
    Misc::printMessage("База данных: " + dbname);
    Misc::printMessage("Пользователь БД: " + dbuser);
    Misc::printMessage("Кодировка: " + db_character_set);
    Misc::printMessage("Для работы с БД используется ODBC");
    Misc::printMessage("Изменить параметры сервера можно в файле .console_chat/server.ini");
    Misc::printMessage("\nПодключение к серверу БД...");
    std::wstring w_connect_data = Misc::string_to_wstring(connect_data);

    constexpr auto SQL_RETURN_CODE_LEN = 1024;
    SQLWCHAR retconstring[SQL_RETURN_CODE_LEN]{}; // строка для кода возврата из функций API ODBC

    // Выделяем дескриптор для базы данных
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))
    {
        diagInfo(SQL_HANDLE_ENV, sqlEnvHandle, "SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle)", "no query");
        wrongDescriptorMsg();
        return false;
    }

    if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
    {
        diagInfo(SQL_HANDLE_ENV, sqlEnvHandle, "SQLSetEnvAttr", "no query");
        wrongDescriptorMsg();
        return false;
    }

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))
    {
        diagInfo(SQL_HANDLE_ENV, sqlEnvHandle, "SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle)", "no query");
        wrongDescriptorMsg();
        return false;
    }

    // Устанавливаем соединение с сервером
    switch (SQLDriverConnectW(sqlConnHandle,
                              GetDesktopWindow(),
                              (SQLWCHAR *)w_connect_data.data(),
                              SQL_NTS,
                              retconstring,
                              1024,
                              NULL,
                              SQL_DRIVER_COMPLETE))
    {

    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
        Misc::printMessage("Успешное подключение к SQL Server");
        Misc::printMessage("server> ", false);
        break;

    case SQL_INVALID_HANDLE:
    case SQL_ERROR:
        diagInfo(SQL_HANDLE_DBC, sqlConnHandle, "SQLDriverConnect", "");
        Misc::printMessage("Не удалось подключиться к SQL Server");
        Misc::printMessage("server> ", false);
        complete();
        return false;

    default:
        break;
    }
    return true;
}

std::shared_ptr<User> ODBC::getUserByID(const ullong &userID)
{
    db_errno = 0;
    if (userID == 0)
        return nullptr;
    std::string query = "SELECT * FROM `users` INNER JOIN hash_tab ON `users`.id = `hash_tab`.uid WHERE `id` = '" + std::to_string(userID) + "' LIMIT 1;";
    int res = dbQuery(query);
    if (res <= 0)
        return nullptr;
    auto user = fetchUserRow(db_errno);
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    return user;
}

std::shared_ptr<User> ODBC::getUserByLogin(const std::string &userLogin)
{
    db_errno = 0;
    std::string query = "SELECT * FROM `users` INNER JOIN hash_tab ON `users`.id = `hash_tab`.uid WHERE `login` LIKE '" + userLogin + "' LIMIT 1;";
    int res = dbQuery(query);
    if (res <= 0)
        return nullptr;
    auto user = fetchUserRow(db_errno);
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    return user;
}

ullong ODBC::getCount(std::string table, std::string where)
{
    db_errno = 0;
    ullong count = 0;
    std::string query = "SELECT COUNT(*) FROM `" + table + "` WHERE " + where + ";";
    int res = dbQuery(query);
    if (res <= SQL_SUCCESS)
    {
        return 0;
    }

    try
    {
        BindCol(sqlStmtHandle, 1, SQL_INTEGER, &res, sizeof(res), nullptr);
    }
    catch (BindColException &e)
    {
        db_errno = 1;
        std::cout << '\n'
                  << e.what() << '\n';
        Misc::printMessage("getCount");
        Misc::printMessage("server> ", false);
        return 0;
    }
    auto called = "getCount " + query;
    if (Fetch(db_errno, called) != SQL_SUCCESS)
        return 0;
    return res;
}

std::string ODBC::userList(ullong &start, ullong &per_page, ullong &capacity)
{
    db_errno = 0;
    std::string query;

    ullong count = getCount("users", "`id` != 0");

    Misc::alignPaginator(start, per_page, count);

    query = "SELECT * FROM `users` WHERE `id` != 0 LIMIT " + std::to_string(start - 1) + ", " + std::to_string(start + per_page) + ";";

    uint res = dbQuery(query);
    capacity = (ullong)res;

    if (res <= 0)
        return std::string();
    std::string result;
    for (int i{0}; i < res; i++)
    {
        result += std::to_string(i + start) + ". "; // порядковый номер
        auto user = fetchUserRow(db_errno, 1, false);
        result += user->userData();
        result += "\n";
    }
    return result;
}

bool ODBC::saveUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy)
{
    db_errno = 0;
    std::string query;

    query = "`login` LIKE '" + user->getLogin() + "' AND `id` != " + std::to_string(user->getID()) + ";";
    uint res = getCount("users", query);

    if (res > 0)
    {
        login_busy = true;
        return false;
    }

    query = "`email` LIKE '" + user->getEmail() + "' AND `id` != " + std::to_string(user->getID()) + ";";
    res = getCount("users", query);
    if (res > 0)
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

    res = dbQuery(query);
    if (res < 0)
        return false;
    res = dbQuery(query2);
    if (res < 0)
        return false;
    return true;
}

bool ODBC::saveUser(std::shared_ptr<User> &user)
{
    db_errno = 0;
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

    std::string query2 = "UPDATE `hash_tab` SET `hash` = '" + user->getHash() + "', `salt` = '" + user->getSalt() + "' WHERE `hash_tab`.`uid` = " + std::to_string(user->getID()) + ";";

    uint res = dbQuery(query);
    if (res < 0)
        return false;
    res = dbQuery(query2);
    if (res < 0)
        return false;
    return true;
}

bool ODBC::addUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy)
{
    db_errno = 0;
    std::string query;

    query = "`login` LIKE '" + user->getLogin() + "';";
    uint res = getCount("users", query);

    if (res > 0)
    {
        login_busy = true;
        return false;
    }

    query = "`email` LIKE '" + user->getEmail() + "';";
    res = getCount("users", query);
    if (res > 0)
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

    res = dbQuery(query);
    if (res < 1)
    {
        return false;
    }

    // согласно задания хеши паролей должны быть в отдельной таблице с триггером
    // из за этого приходится у нового пользователя получить автоинкрементный id а потом добавлять хеши
    // если бы хеш хранился в таблице пользователя, можно было бы его засунуть в предыдущем запросе.
    auto user_in_db = getUserByLogin(user->getLogin());
    query = "UPDATE `hash_tab` SET `hash` = '" + user->getHash() + "', `salt` = '" + user->getSalt() + "' WHERE `hash_tab`.`uid` = " + std::to_string(user_in_db->getID()) + ";";
    res = dbQuery(query);
    if (res < 1)
    {
        return false;
    }
    user = getUserByID(user_in_db->getID());
    return true;
}

std::shared_ptr<Message> ODBC::getMessageByID(const ullong &messageID)
{
    /*этот метод скорее сего будет не нужен*/
    return std::shared_ptr<Message>();
}

bool ODBC::addMessage(std::shared_ptr<Message> &message)
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
    int res = dbQuery(query);
    if (res < 1)
    {
        return false;
    }
    return true;
}

std::string ODBC::messageList(ullong &reader_id, ullong &start, ullong &per_page, ullong &capacity)
{
    db_errno = 0;
    std::string query;
    std::string queryUpd = "UPDATE `pub_messages` SET `status` = (`status`| " + std::to_string(msg::message_read) + ") & ~ " +
                           std::to_string(msg::message_delivered) +
                           " WHERE (`author_id` != " + std::to_string(reader_id) + ") AND (";

    ullong count = getCount("pub_messages", "1");

    Misc::alignPaginator(start, per_page, count);

    query = "SELECT * FROM `pub_messages` INNER JOIN `users` ON `pub_messages`.`author_id` = `users`.`id` ORDER BY `pub_messages`.`published` ASC LIMIT " + std::to_string(start - 1) + ", " + std::to_string(per_page) + ";";

    capacity = dbQuery(query);

    std::string result;
    for (ullong i = 0; i < capacity; i++)
    {
        result += std::to_string(i + start) + ". Сообщение\n"; // порядковый номер
        std::shared_ptr<User> user;
        std::shared_ptr<Message> msg;
        fetchMessageRow(user, msg);

        if (user != nullptr && msg != nullptr)
        {
            result += user->userData() + "\n";
            result += msg->messageData();
            result += "\n\n";

            queryUpd += "`id` = " + std::to_string(msg->getID());
            if (i + 1 == capacity)
                queryUpd += ");";
            else
                queryUpd += " OR ";
        }
        else
        {
            result += "Не удалось получить сообщение из базы данных. Возможная причина: нестандарные символы в тексте сообщения. Переключите сервер в режим MySQL API\n\n";
        }
    }
    if (capacity > 0)
        dbQuery(queryUpd);
    return result;
}

std::string ODBC::messageList(ullong &reader_id, ullong interlocutor_id, ullong &start, ullong &per_page, ullong &capacity)
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
    capacity = dbQuery(query);
    std::string result;
    for (ullong i = 0; i < capacity; i++)
    {
        result += std::to_string(i + start) + ". Сообщение\n"; // порядковый номер
        std::shared_ptr<User> user;
        std::shared_ptr<Message> msg;
        fetchMessageRow(user, msg, 1, false);
        if (user != nullptr && msg != nullptr)
        {
            result += user->userData() + "\n";
            result += msg->messageData();

            result += "\n\n";

            queryUpd += "`id` = " + std::to_string(msg->getID());
            if (i + 1 == capacity)
                queryUpd += ");";
            else
                queryUpd += " OR ";
        }
        else
        {
            result += "Не удалось получить сообщение из базы данных. Возможная причина: нестандартные символы в тексте сообщения. Переключите сервер в режим MySQL API\n\n";
        }
    }
    if (capacity > 0)
        dbQuery(queryUpd);
    return result;
}

bool ODBC::deleteByID(ullong &id, std::string &&table)
{
    std::string query = "DELETE FROM `" + table + "` WHERE `id` = " + std::to_string(id) + ";";
    return dbQuery(query);
}

bool ODBC::setStatus(ullong &id, std::string &&table, uint action, uint new_status)
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

    return dbQuery(query);
}

void ODBC::DBclose()
{
    complete();
}

void ODBC::hello()
{
    Misc::printMessage("This server uses ODBC.");
    Misc::printMessage("server> ", false);
}

std::shared_ptr<User> ODBC::fetchUserRow(uint &db_errno, uint startCol, bool getPassData)
{
    ullong id;
    wchar_t login[SQL_CHAR_RESULT_LEN];
    wchar_t email[SQL_CHAR_RESULT_LEN];
    wchar_t first_name[SQL_CHAR_RESULT_LEN];
    wchar_t last_name[SQL_CHAR_RESULT_LEN];
    ullong registered;
    int status;
    ullong session_key;
    std::string hash;
    std::string salt;
    wchar_t db_hash[SQL_CHAR_RESULT_LEN];
    wchar_t db_salt[SQL_CHAR_RESULT_LEN];

    SQLLEN login_length;
    SQLLEN email_length;
    SQLLEN first_name_length;
    SQLLEN last_name_length;
    SQLLEN hash_length;
    SQLLEN salt_length;
    try
    {
        BindCol(sqlStmtHandle, startCol, SQL_C_UBIGINT, &id, sizeof(id), nullptr);
        BindCol(sqlStmtHandle, ++startCol, SQL_WCHAR, &login, SQL_CHAR_RESULT_LEN, &login_length);
        BindCol(sqlStmtHandle, ++startCol, SQL_WCHAR, &email, SQL_CHAR_RESULT_LEN, &email_length);
        BindCol(sqlStmtHandle, ++startCol, SQL_WCHAR, &first_name, SQL_CHAR_RESULT_LEN, &first_name_length);
        BindCol(sqlStmtHandle, ++startCol, SQL_WCHAR, &last_name, SQL_CHAR_RESULT_LEN, &last_name_length);
        BindCol(sqlStmtHandle, ++startCol, SQL_C_UBIGINT, &registered, sizeof(registered), nullptr);
        BindCol(sqlStmtHandle, ++startCol, SQL_INTEGER, &status, sizeof(status), nullptr);
        BindCol(sqlStmtHandle, ++startCol, SQL_C_UBIGINT, &session_key, sizeof(session_key), nullptr);

        if (getPassData)
        {
            ++startCol;
            BindCol(sqlStmtHandle, ++startCol, SQL_WCHAR, &db_hash, SQL_CHAR_RESULT_LEN, &hash_length);
            BindCol(sqlStmtHandle, ++startCol, SQL_WCHAR, &db_salt, SQL_CHAR_RESULT_LEN, &salt_length);
        }
    }
    catch (BindColException &e)
    {
        db_errno = 1;
        std::cout << '\n'
                  << e.what() << '\n';
        Misc::printMessage("Ошибка вызвана функцией fetchUserRow");
        Misc::printMessage("server> ", false);
    }
    std::string called = "fetchUserRow вызвала Fetch";
    int res = Fetch(db_errno, called);

    if (res == SQL_SUCCESS)
    {

        if (getPassData)
        {
            hash = Misc::wstring_to_string(std::wstring(db_hash, hash_length / 2));
            salt = Misc::wstring_to_string(std::wstring(db_salt, salt_length / 2));
        }

        auto user = std::make_shared<User>(
            id,
            Misc::wstring_to_string(std::wstring(login, login_length / 2)),
            Misc::wstring_to_string(std::wstring(email, email_length / 2)),
            Misc::wstring_to_string(std::wstring(first_name, first_name_length / 2)),
            Misc::wstring_to_string(std::wstring(last_name, last_name_length / 2)),
            registered,
            (user::status)status,
            session_key,
            hash,
            salt);
        return user;
    }

    return nullptr;
}

void ODBC::fetchMessageRow(std::shared_ptr<User> &user, std::shared_ptr<Message> &msg, uint startCol, bool pub)
{
    ullong id;
    ullong author_id;
    ullong recipient_id;
    wchar_t text[SQL_CHAR_BIG_RESULT_LEN];
    ullong published;
    int status;

    SQLLEN text_len;
    try
    {
        BindCol(sqlStmtHandle, startCol, SQL_C_UBIGINT, &id, sizeof(id), nullptr);
        BindCol(sqlStmtHandle, ++startCol, SQL_C_UBIGINT, &author_id, sizeof(author_id), nullptr);
        if (!pub)
            BindCol(sqlStmtHandle, ++startCol, SQL_C_UBIGINT, &recipient_id, sizeof(recipient_id), nullptr);
        BindCol(sqlStmtHandle, ++startCol, SQL_WCHAR, &text, SQL_CHAR_BIG_RESULT_LEN, &text_len);
        BindCol(sqlStmtHandle, ++startCol, SQL_C_UBIGINT, &published, sizeof(published), nullptr);
        BindCol(sqlStmtHandle, ++startCol, SQL_INTEGER, &status, sizeof(status), nullptr);
    }
    catch (BindColException &e)
    {
        db_errno = 1;
        std::cout << '\n'
                  << e.what() << '\n';
        Misc::printMessage("Ошибка вызвана функцией fetchMessageRow");
        Misc::printMessage("server> ", false);
        user = nullptr;
        msg = nullptr;
    }
    uint userCol = pub ? 6 : 7;
    user = fetchUserRow(db_errno, userCol, false);

    if (user != nullptr)
    {
        if (pub)
        {
            msg = std::make_shared<Message>(id, author_id, Misc::wstring_to_string(std::wstring(text, text_len / 2)), published, status);
        }
        else
        {
            msg = std::make_shared<Message>(id, author_id, recipient_id, Misc::wstring_to_string(std::wstring(text, text_len / 2)), published, status);
        }
    }
    else
    {
        user = nullptr;
        msg = nullptr;
    }

    return;
}

int ODBC::dbQuery(std::string &query)
{
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    db_errno = 0;

    SQLINTEGER res = SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle);
    if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO)
    {
        diagInfo(SQL_HANDLE_STMT, sqlStmtHandle, "SQLAllocHandle in dbQuery sqlStmtHandle", query);
        diagInfo(SQL_HANDLE_DBC, sqlConnHandle, "SQLAllocHandle in dbQuery sqlConnHandle", query);
        db_errno = 1;
        return -1;
    }
    std::wstring w_query = Misc::string_to_wstring(query);
    int result = SQLExecDirectW(sqlStmtHandle, (SQLWCHAR *)w_query.data(), SQL_NTS);
    if (result != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO)
    {
        diagInfo(SQL_HANDLE_STMT, sqlStmtHandle, "SQLExecDirectA", query);
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        Misc::printMessage("ODBC: Failed to complete the query " + query);
        db_errno = 1;
        return -1;
    }
    SQLLEN RowCountPtr = -1;
    if (result == SQL_SUCCESS)
        SQLRowCount(sqlStmtHandle, &RowCountPtr);

    if (res == SQL_SUCCESS_WITH_INFO)
    {
        diagInfo(SQL_HANDLE_STMT, sqlStmtHandle, "dbQuery", query);
    }

    return RowCountPtr;
}

SQLINTEGER ODBC::Fetch(uint &db_errno, std::string &called)
{
    db_errno = 0;
    int res = SQLFetch(sqlStmtHandle);
    if (res != SQL_SUCCESS && res != SQL_NO_DATA && res != SQL_SUCCESS_WITH_INFO)
    {
        Misc::printMessage(called);
        diagInfo(SQL_HANDLE_STMT, sqlStmtHandle, "Fetch", "");
        db_errno = 1;
    }

    if (res == SQL_SUCCESS_WITH_INFO)
    {
        diagInfo(SQL_HANDLE_STMT, sqlStmtHandle, "ODBC::Fetch", called);
    }

    return res;
}

void ODBC::BindCol(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr)
{
    SQLRETURN res = SQLBindCol(StatementHandle, ColumnNumber, TargetType, TargetValuePtr, BufferLength, StrLen_or_IndPtr);
    if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO)
    {
        diagInfo(SQL_HANDLE_STMT, StatementHandle, "SQLBindCol", "");
        throw BindColException();
    }
    if (res == SQL_SUCCESS_WITH_INFO)
    {
        diagInfo(SQL_HANDLE_STMT, StatementHandle, "SQLBindCol", "");
    }
}

void ODBC::diagInfo(SQLINTEGER handle_type, SQLHANDLE &handle, const std::string &function, const std::string &query)
{
    SQLCHAR Sqlstate[6];
    for (int i{0}; i < 6; i++)
    {
        Sqlstate[i] = '0';
    }
    SQLINTEGER NativeErrorPtr = 0;
    SQLCHAR MessageText[2048];
    for (int i{0}; i < 2048; i++)
    {
        MessageText[i] = '\0';
    }
    SQLSMALLINT MessageText_len = 0;
    SQLGetDiagRecA(handle_type, handle, 1, Sqlstate, &NativeErrorPtr, MessageText, 2048, &MessageText_len);

    std::string sqlstate;
    std::string msg;

    for (int i{0}; i < 5; i++)
    {
        sqlstate.push_back(Sqlstate[i]);
    }
    for (int i{0}; i < MessageText_len + 1; i++)
    {
        /* SQLGetDiagRec может записывать в сообщение неопределенные байты
         из за чего программа вылетает при преобразовании сообщения в wchar_t
         Оставим только ASCII символы
        */
        if (MessageText[i] >= 0 && MessageText[i] < 127)
            msg.push_back(MessageText[i]);
    }
    Misc::printMessage("\nФункция: " + function);
    Misc::printMessage("Запрос: " + query);
    Misc::printMessage("Sqlstate: " + sqlstate + " Расшифровка https://learn.microsoft.com/ru-ru/sql/odbc/reference/syntax/odbc-api-reference?view=sql-server-ver16");
    Misc::printMessage("NativeError: " + std::to_string((int)NativeErrorPtr));
    Misc::printMessage("Message: " + msg);
    Misc::printMessage("server> ", false);
    return;
}
#endif