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

    /// @brief Инициализация подключения к базе
    virtual bool initialize() = 0;

    /// @brief Получить пользователя из базы по ID
    virtual std::shared_ptr<User> getUserByID(const ullong &userID) = 0;

    /// @brief Получить пользователя по логину
    virtual std::shared_ptr<User> getUserByLogin(const std::string &userLogin) = 0;

    /// @brief Получить количество рядов в запросе
    virtual ullong getCount(std::string table, std::string where) = 0;

    /// @brief Вывести список пользователей
    virtual std::string userList(ullong &start, ullong &per_page, ullong &capacity) = 0;

    /// @brief Сохранить пользователя если он уже есть в базе с проверкой уникальности логина и email
    virtual bool saveUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy) = 0;

    /// @brief Сохранить пользователя если он уже есть в базе без проверки уникальности логина и email
    virtual bool saveUser(std::shared_ptr<User> &user) = 0;

    /// @brief Добавить нового пользователя в базу
    virtual bool addUser(std::shared_ptr<User> &user, bool &login_busy, bool &email_busy) = 0;

    /// @brief Получает сообщение из базы
    virtual std::shared_ptr<Message> getMessageByID(const ullong &messageID) = 0;

    /// @brief Добавляет новое сообщение в базу
    virtual bool addMessage(std::shared_ptr<Message> &message) = 0;

    /// @brief Получает список публичных сообщений
    /// @param reader_id сообщение будет помечено прочитанным если этот ID не является автором сообщения.
    /// @param start порядковый номер сообщения
    /// @param per_page сообщений на страницу
    /// @param capacity количество найденных сообщений
    virtual std::string messageList(ullong &reader_id, ullong &start, ullong &per_page, ullong &capacity) = 0;

    /// @brief Получает список личных сообщений
    /// @param reader_id сообщение будет помечено прочитанным если этот ID не является автором сообщения.
    virtual std::string messageList(ullong &reader_id, ullong interlocutor_id, ullong &start, ullong &per_page, ullong &capacity) = 0;

    /// @brief удаление элемента по ID
    virtual bool deleteByID(ullong &id, std::string &&table) = 0;

    /// @brief изменение флагов статуса
    /// @param action 0 - удалить флаг; 1 - добавить флаг; 3 - очистить и задать новые флаги
    virtual bool setStatus(ullong &id, std::string &&table, uint action, uint new_status) = 0;

    virtual void DBclose() = 0;

    uint getDBerrno() { return db_errno; }
    void clearDBerrno() { db_errno = 0; }

    /// @brief тестовая функция, указывает какой тип подключения к базе
    virtual void hello() = 0;
};