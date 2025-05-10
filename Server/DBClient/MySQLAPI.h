#pragma once
#include "DBCore.h"
#include <cstdlib>
#if defined(__linux__)
#include </usr/include/mysql/mysql.h>
#endif
#if defined(_WIN64) || defined(_WIN32)
#include <mysql.h>
#pragma comment(lib, "libmysql.lib")
#endif
class MySQLAPI final : public DBCore
{
private:
    MYSQL mysql;
    MYSQL_RES *res;
    MYSQL_ROW row;
    uint select_count = 0;
    MYSQL_ROW_OFFSET select_offset;
    void select() { select_count = 0; }

public:
    ~MySQLAPI() = default;
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
    std::shared_ptr<User> fetchUserRow(uint startRow = 0, bool getPassData = true);
    std::shared_ptr<Message> fetchMessageRow(uint startRow = 0, bool pub = true);
    uint querySelect(std::string &query);
    uint queryUpdate(std::string &query);
};
