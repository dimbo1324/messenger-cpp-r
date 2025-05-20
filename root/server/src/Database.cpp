#include "Database.h"
#include <iostream>
#include <memory>

Database::Database(const std::string &connStr)
    : conn_(std::make_unique<pqxx::connection>(connStr))
{
    try
    {
        if (conn_->is_open())
        {
            std::cout << "Подключено к базе данных: " << conn_->dbname() << std::endl;
        }
        else
        {
            throw std::runtime_error("Не удалось подключиться к базе данных");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Ошибка подключения: " << e.what() << std::endl;
        throw;
    }
}

Database::~Database() = default;

bool Database::registerUser(const std::string &login, const std::string &password)
{
    try
    {
        pqxx::work txn(*conn_);
        txn.exec(
            pqxx::zview("INSERT INTO chat.users (login, password) VALUES ($1, $2)"),
            pqxx::params{login, password});
        txn.commit();
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Ошибка регистрации: " << e.what() << std::endl;
        return false;
    }
}

bool Database::loginUser(const std::string &login, const std::string &password)
{
    try
    {
        pqxx::work txn(*conn_);
        pqxx::result res = txn.exec(
            pqxx::zview("SELECT id FROM chat.users WHERE login = $1 AND password = $2"),
            pqxx::params{login, password});
        if (!res.empty())
        {
            int userId = res[0][0].as<int>();
            txn.exec(
                pqxx::zview("INSERT INTO chat.online_status (user_id, status) VALUES ($1, 'онлайн') "
                            "ON CONFLICT (user_id) DO UPDATE SET status = 'онлайн'"),
                pqxx::params{userId});
            txn.commit();
            return true;
        }
        return false;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Ошибка входа: " << e.what() << std::endl;
        return false;
    }
}

void Database::sendMessage(int senderId, int receiverId, const std::string &text)
{
    try
    {
        pqxx::work txn(*conn_);
        txn.exec(
            pqxx::zview("INSERT INTO chat.messages (sender_id, receiver_id, text) VALUES ($1, $2, $3)"),
            pqxx::params{senderId, receiverId, text});
        txn.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Ошибка отправки сообщения: " << e.what() << std::endl;
    }
}

std::vector<std::string> Database::getInbox(int userId)
{
    std::vector<std::string> messages;
    try
    {
        pqxx::work txn(*conn_);
        pqxx::result res = txn.exec(
            pqxx::zview("SELECT m.text, u.login FROM chat.messages m JOIN chat.users u ON m.sender_id = u.id WHERE m.receiver_id = $1"),
            pqxx::params{userId});
        for (const auto &row : res)
        {
            messages.push_back(row[1].as<std::string>() + ": " + row[0].as<std::string>());
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Ошибка получения входящих сообщений: " << e.what() << std::endl;
    }
    return messages;
}

void Database::logEvent(const std::string &eventType, int userId, const std::string &details)
{
    try
    {
        pqxx::work txn(*conn_);
        txn.exec(
            pqxx::zview("INSERT INTO chat.logs (event_type, user_id, details) VALUES ($1, $2, $3)"),
            pqxx::params{eventType, userId, details});
        txn.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Ошибка записи лога: " << e.what() << std::endl;
    }
}