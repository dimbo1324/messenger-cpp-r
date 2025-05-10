#pragma once
#if defined(_WIN64) || defined(_WIN32)
#include "DBCore.h"
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <exception>

constexpr auto SQL_CHAR_RESULT_LEN = 2048;
constexpr auto SQL_CHAR_BIG_RESULT_LEN = 200000;

class ODBC final : public DBCore
{
private:
    std::string connect_data;

    SQLHANDLE sqlConnHandle{nullptr}; // дескриптор для соединения с базой данных
    SQLHANDLE sqlEnvHandle{nullptr};  // дескриптор окружения базы данных
    SQLHANDLE sqlStmtHandle{nullptr}; // дескриптор для выполнения запросов к базе данных

    int SQL_RESULT_LEN = 240;
    int SQL_RETURN_CODE_LEN = 1024;

public:
    ~ODBC() = default;
    /// @brief инициализация подключения к базе данных.
    bool initialize() override;

    /// @brief Получает указатель пользователя по ID
    std::shared_ptr<User> getUserByID(const ullong &userID) override;

    /// @brief Получает указатель пользователя по логину
    std::shared_ptr<User> getUserByLogin(const std::string &userLogin) override;

    /// @brief Получает количество элементов с запросе SELECT COUNT(*)
    ullong getCount(std::string table, std::string where) override;

    /// @brief Выводит список пользователей в строку
    std::string userList(ullong &start, ullong &per_page, ullong &capacity) override;

    /// @brief Сохраняет пользователя если он есть в базе
    /// @param login_busy true если логин занят. Сохранение отменяется.
    /// @param email_busy true если email занят. Сохранение отменяется.
    bool saveUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy) override;

    /// @brief Сохранить пользователя если он уже есть в базе без проверки уникальности логина и email
    bool saveUser(std::shared_ptr<User> &user) override;

    /// @brief Добавляет нового пользователя в базу
    /// @param login_busy true если логин занят. Добавление отменяется.
    /// @param email_busy true если email занят. Добавление отменяется.
    bool addUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy) override;

    /// @brief Получает сообщение по ID. Не работает. Возможно будет удалена.
    std::shared_ptr<Message> getMessageByID(const ullong &messageID) override;

    /// @brief Добавляет сообщение в базу данных
    bool addMessage(std::shared_ptr<Message> &message) override;

    /// @brief Выводит список публичных сообщений
    /// @param reader_id Если читатель не является автором, сообщение будет помечено прочитанным.
    /// @param capacity выводит количество сообщений в списке
    std::string messageList(ullong &reader_id, ullong &start, ullong &per_page, ullong &capacity) override;

    /// @brief Выводит список личных сообщений
    std::string messageList(ullong &reader_id, ullong interlocutor_id, ullong &start, ullong &per_page, ullong &capacity) override;

    /// @brief удаление элемента по ID
    bool deleteByID(ullong &id, std::string &&table) override;

    /// @brief изменение флагов статуса
    /// @param action 0 - удалить флаг; 1 - добавить флаг; 3 - очистить и задать новые флаги
    bool setStatus(ullong &id, std::string &&table, uint action, uint new_status) override;

    void DBclose() override;

    /// @brief тестовая функция, указывает какой тип подключения к базе
    void hello() override;

private:
    std::shared_ptr<User> fetchUserRow(uint &db_errno, uint startCol = 1, bool getPassData = true);
    void fetchMessageRow(std::shared_ptr<User> &user, std::shared_ptr<Message> &msg, uint startCol = 1, bool pub = true);

    /// @brief Выполняет запрос
    /// @return количество обработанных рядов или -1 если операция не затрагивает ряды.
    int dbQuery(std::string &query);

    SQLINTEGER Fetch(uint &db_errno, std::string &called);

    void BindCol(
        SQLHSTMT StatementHandle,
        SQLUSMALLINT ColumnNumber,
        SQLSMALLINT TargetType,
        SQLPOINTER TargetValuePtr,
        SQLLEN BufferLength,
        SQLLEN *StrLen_or_IndPtr);

    void diagInfo(SQLINTEGER handle_type, SQLHANDLE &handle, const std::string &function, const std::string &query);

    void wrongDescriptorMsg()
    {
        Misc::printMessage("Не удалось создать дескриптор ODBC");
    }
    void complete()
    {
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        SQLDisconnect(sqlConnHandle);
        SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
        SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);
    }
};

class BindColException : public std::exception
{
public:
    virtual const char *what() const noexcept override
    {
        return "ERROR: SQLBindColException!";
    }
};

#endif