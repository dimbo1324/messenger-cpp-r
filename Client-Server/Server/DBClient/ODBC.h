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
    SQLHANDLE sqlConnHandle{nullptr};
    SQLHANDLE sqlEnvHandle{nullptr};
    SQLHANDLE sqlStmtHandle{nullptr};
    int SQL_RESULT_LEN = 240;
    int SQL_RETURN_CODE_LEN = 1024;

public:
    ~ODBC() = default;
    bool initialize() override;
    std::shared_ptr<User> getUserByID(const ullong &userID) override;
    std::shared_ptr<User> getUserByLogin(const std::string &userLogin) override;
    ullong getCount(std::string table, std::string where) override;
    std::string userList(ullong &start, ullong &per_page, ullong &capacity) override;
    bool saveUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy) override;
    bool saveUser(std::shared_ptr<User> &user) override;
    bool addUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy) override;
    std::shared_ptr<Message> getMessageByID(const ullong &messageID) override;
    bool addMessage(std::shared_ptr<Message> &message) override;
    std::string messageList(ullong &reader_id, ullong &start, ullong &per_page, ullong &capacity) override;
    std::string messageList(ullong &reader_id, ullong interlocutor_id, ullong &start, ullong &per_page, ullong &capacity) override;
    bool deleteByID(ullong &id, std::string &&table) override;
    bool setStatus(ullong &id, std::string &&table, uint action, uint new_status) override;
    void DBclose() override;
    void hello() override;

private:
    std::shared_ptr<User> fetchUserRow(uint &db_errno, uint startCol = 1, bool getPassData = true);
    void fetchMessageRow(std::shared_ptr<User> &user, std::shared_ptr<Message> &msg, uint startCol = 1, bool pub = true);
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