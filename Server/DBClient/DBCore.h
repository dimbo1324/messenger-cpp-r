#pragma once
#include <string>
#include <memory>
#include <iostream>
#include "../../Misc/Misc.h"
#include "../ChatCore/User.h"
#include "../ChatCore/Message.h"
typedef unsigned int uint;
typedef unsigned long long ullong;
const extern char config_file[];
class DBCore
{
protected:
    std::string server;
    std::string port;
    std::string dbuser;
    std::string dbpass;
    std::string dbname;
    std::string db_character_set;
    uint db_errno = 0;

public:
    int provider;
    DBCore();
    virtual ~DBCore() = default;
    virtual bool initialize() = 0;
    virtual std::shared_ptr<User> getUserByID(const ullong &userID) = 0;
    virtual std::shared_ptr<User> getUserByLogin(const std::string &userLogin) = 0;
    virtual ullong getCount(std::string table, std::string where) = 0;
    virtual std::string userList(ullong &start, ullong &per_page, ullong &capacity) = 0;
    virtual bool saveUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy) = 0;
    virtual bool saveUser(std::shared_ptr<User> &user) = 0;
    virtual bool addUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy) = 0;
    virtual std::shared_ptr<Message> getMessageByID(const ullong &messageID) = 0;
    virtual bool addMessage(std::shared_ptr<Message> &message) = 0;
    virtual std::string messageList(ullong &reader_id, ullong &start, ullong &per_page, ullong &capacity) = 0;
    virtual std::string messageList(ullong &reader_id, ullong interlocutor_id, ullong &start, ullong &per_page, ullong &capacity) = 0;
    virtual bool deleteByID(ullong &id, std::string &&table) = 0;
    virtual bool setStatus(ullong &id, std::string &&table, uint action, uint new_status) = 0;
    virtual void DBclose() = 0;
    uint getDBerrno() { return db_errno; }
    void clearDBerrno() { db_errno = 0; }
    virtual void hello() = 0;
};