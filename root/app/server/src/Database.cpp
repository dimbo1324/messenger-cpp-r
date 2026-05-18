#include "Database.h"
#include "Auth.h"
#include "Logger.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace
{
    constexpr int kDefaultLimit = 50;
    constexpr int kMaxLimit = 500;

    int clampLimit(int limit)
    {
        if (limit <= 0)
        {
            return kDefaultLimit;
        }
        return std::min(limit, kMaxLimit);
    }

    std::string timestampExpr(const std::string &column)
    {
        return "to_char(" + column + " AT TIME ZONE 'UTC', 'YYYY-MM-DD\"T\"HH24:MI:SS\"Z\"')";
    }

    DbResult makeResult(DbStatus status, std::string message = {}, int id = -1)
    {
        return DbResult{status, std::move(message), id};
    }
}

DatabasePool::Connection::Connection(DatabasePool &pool, std::unique_ptr<pqxx::connection> connection)
    : pool_(&pool), connection_(std::move(connection))
{
}

DatabasePool::Connection::~Connection()
{
    if (pool_ != nullptr && connection_ != nullptr)
    {
        pool_->release(std::move(connection_));
    }
}

DatabasePool::Connection::Connection(Connection &&other) noexcept
    : pool_(other.pool_), connection_(std::move(other.connection_))
{
    other.pool_ = nullptr;
}

DatabasePool::Connection &DatabasePool::Connection::operator=(Connection &&other) noexcept
{
    if (this != &other)
    {
        if (pool_ != nullptr && connection_ != nullptr)
        {
            pool_->release(std::move(connection_));
        }
        pool_ = other.pool_;
        connection_ = std::move(other.connection_);
        other.pool_ = nullptr;
    }
    return *this;
}

pqxx::connection &DatabasePool::Connection::get()
{
    return *connection_;
}

DatabasePool::DatabasePool(std::string connStr, std::size_t poolSize)
    : connStr_(std::move(connStr))
{
    if (poolSize == 0)
    {
        throw std::runtime_error("database pool size must be greater than zero");
    }

    for (std::size_t i = 0; i < poolSize; ++i)
    {
        auto connection = std::make_unique<pqxx::connection>(connStr_);
        if (!connection->is_open())
        {
            throw std::runtime_error("failed to open PostgreSQL connection");
        }
        available_.push(std::move(connection));
    }
}

DatabasePool::Connection DatabasePool::acquire()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !available_.empty(); });
    auto connection = std::move(available_.front());
    available_.pop();
    return Connection(*this, std::move(connection));
}

std::size_t DatabasePool::size() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return available_.size();
}

void DatabasePool::release(std::unique_ptr<pqxx::connection> connection)
{
    if (connection == nullptr)
    {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        available_.push(std::move(connection));
    }
    cv_.notify_one();
}

Database::Database(const std::string &connStr, std::size_t poolSize)
    : pool_(connStr, poolSize)
{
}

bool Database::ping()
{
    try
    {
        auto conn = pool_.acquire();
        pqxx::read_transaction txn(conn.get());
        txn.exec("SELECT 1");
        return true;
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("database ping failed: ") + e.what());
        return false;
    }
}

std::string Database::hashPassword(const std::string &plainPassword)
{
    auto conn = pool_.acquire();
    pqxx::work txn(conn.get());
    auto hash = hashPassword(txn, plainPassword);
    txn.commit();
    return hash;
}

bool Database::verifyPassword(const std::string &plainPassword, const std::string &passwordHash)
{
    auto conn = pool_.acquire();
    pqxx::work txn(conn.get());
    const bool ok = verifyPassword(txn, plainPassword, passwordHash);
    txn.commit();
    return ok;
}

std::string Database::hashPassword(pqxx::work &txn, const std::string &plainPassword)
{
    const pqxx::result res = txn.exec(
        pqxx::zview("SELECT crypt($1, gen_salt('bf', 12))"),
        pqxx::params{plainPassword});
    return res[0][0].as<std::string>();
}

bool Database::verifyPassword(pqxx::work &txn,
                              const std::string &plainPassword,
                              const std::string &passwordHash)
{
    const pqxx::result res = txn.exec(
        pqxx::zview("SELECT crypt($1, $2) = $2"),
        pqxx::params{plainPassword, passwordHash});
    return res[0][0].as<bool>();
}

DbResult Database::registerUser(const std::string &login, const std::string &password)
{
    if (!auth::isValidLogin(login))
    {
        return makeResult(DbStatus::ValidationError,
                          "login must be 3-50 chars: letters, digits, '.', '_' or '-'");
    }

    const auto policy = auth::validatePassword(password);
    if (!policy.ok)
    {
        return makeResult(DbStatus::ValidationError, policy.message);
    }

    try
    {
        auto conn = pool_.acquire();
        pqxx::work txn(conn.get());
        const std::string passwordHash = hashPassword(txn, password);
        const pqxx::result res = txn.exec(
            pqxx::zview("INSERT INTO chat.users (login, password_hash, role, status) "
                        "VALUES ($1, $2, 'user', 'offline') RETURNING id"),
            pqxx::params{login, passwordHash});
        const int id = res[0][0].as<int>();
        audit(txn, id, "register", login, "user self-registered");
        txn.commit();
        Logger::getInstance().info("user registered: " + login);
        return makeResult(DbStatus::Ok, "registered", id);
    }
    catch (const pqxx::unique_violation &)
    {
        return makeResult(DbStatus::Duplicate, "login already exists");
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("registration failed for ") + login + ": " + e.what());
        return makeResult(DbStatus::Error, "database error");
    }
}

LoginResult Database::loginUser(const std::string &login, const std::string &password)
{
    try
    {
        auto conn = pool_.acquire();
        pqxx::work txn(conn.get());
        const pqxx::result res = txn.exec(
            pqxx::zview("SELECT id, password_hash, role, status FROM chat.users WHERE login = $1"),
            pqxx::params{login});

        if (res.empty())
        {
            return {DbStatus::Unauthorized, "invalid login or password"};
        }

        const int userId = res[0][0].as<int>();
        const std::string passwordHash = res[0][1].as<std::string>();
        const std::string role = res[0][2].as<std::string>();
        const std::string status = res[0][3].as<std::string>();

        if (status == "banned")
        {
            return {DbStatus::Banned, "user is banned", userId, login, role};
        }

        if (!verifyPassword(txn, password, passwordHash))
        {
            Logger::getInstance().warn("login failed for user: " + login);
            return {DbStatus::Unauthorized, "invalid login or password"};
        }

        txn.exec(
            pqxx::zview("UPDATE chat.users SET status = 'online', last_seen = now(), updated_at = now() WHERE id = $1"),
            pqxx::params{userId});
        audit(txn, userId, "login", login, "login success");
        txn.commit();

        Logger::getInstance().info("login success: " + login);
        return {DbStatus::Ok, "login ok", userId, login, role};
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("login failed for ") + login + ": " + e.what());
        return {DbStatus::Error, "database error"};
    }
}

DbResult Database::logoutUser(int userId)
{
    try
    {
        auto conn = pool_.acquire();
        pqxx::work txn(conn.get());
        txn.exec(
            pqxx::zview("UPDATE chat.users SET status = 'offline', last_seen = now(), updated_at = now() "
                        "WHERE id = $1 AND status <> 'banned'"),
            pqxx::params{userId});
        txn.commit();
        return makeResult(DbStatus::Ok, "logout ok", userId);
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("logout failed: ") + e.what());
        return makeResult(DbStatus::Error, "database error");
    }
}

std::optional<int> Database::getUserId(const std::string &login)
{
    try
    {
        auto conn = pool_.acquire();
        pqxx::read_transaction txn(conn.get());
        const pqxx::result res = txn.exec(
            pqxx::zview("SELECT id FROM chat.users WHERE login = $1"),
            pqxx::params{login});
        if (res.empty())
        {
            return std::nullopt;
        }
        return res[0][0].as<int>();
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("getUserId failed: ") + e.what());
        return std::nullopt;
    }
}

std::optional<UserSummary> Database::getUserByLogin(const std::string &login)
{
    try
    {
        auto conn = pool_.acquire();
        pqxx::read_transaction txn(conn.get());
        const std::string query =
            "SELECT id, login, role, status, COALESCE(" +
            timestampExpr("last_seen") +
            ", '') FROM chat.users WHERE login = $1";
        const pqxx::result res = txn.exec(pqxx::zview(query.c_str()), pqxx::params{login});
        if (res.empty())
        {
            return std::nullopt;
        }
        return UserSummary{res[0][0].as<int>(),
                           res[0][1].as<std::string>(),
                           res[0][2].as<std::string>(),
                           res[0][3].as<std::string>(),
                           res[0][4].as<std::string>()};
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("getUserByLogin failed: ") + e.what());
        return std::nullopt;
    }
}

std::vector<UserSummary> Database::listUsers()
{
    std::vector<UserSummary> users;
    try
    {
        auto conn = pool_.acquire();
        pqxx::read_transaction txn(conn.get());
        const std::string query =
            "SELECT id, login, role, status, COALESCE(" +
            timestampExpr("last_seen") +
            ", '') FROM chat.users ORDER BY login";
        const pqxx::result res = txn.exec(pqxx::zview(query.c_str()));
        for (const auto &row : res)
        {
            users.push_back(UserSummary{row[0].as<int>(),
                                        row[1].as<std::string>(),
                                        row[2].as<std::string>(),
                                        row[3].as<std::string>(),
                                        row[4].as<std::string>()});
        }
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("listUsers failed: ") + e.what());
    }
    return users;
}

DbResult Database::sendMessage(int senderId, const std::string &receiverLogin, const std::string &text)
{
    if (text.empty() || text.size() > 4096)
    {
        return makeResult(DbStatus::ValidationError, "message must be 1-4096 characters");
    }

    try
    {
        auto conn = pool_.acquire();
        pqxx::work txn(conn.get());
        const pqxx::result receiver = txn.exec(
            pqxx::zview("SELECT id, status FROM chat.users WHERE login = $1"),
            pqxx::params{receiverLogin});
        if (receiver.empty())
        {
            return makeResult(DbStatus::NotFound, "receiver not found");
        }

        if (receiver[0][1].as<std::string>() == "banned")
        {
            return makeResult(DbStatus::Banned, "receiver is banned");
        }

        const int receiverId = receiver[0][0].as<int>();
        const pqxx::result inserted = txn.exec(
            pqxx::zview("INSERT INTO chat.messages (sender_id, receiver_id, text) "
                        "VALUES ($1, $2, $3) RETURNING id"),
            pqxx::params{senderId, receiverId, text});
        const int messageId = inserted[0][0].as<int>();
        txn.commit();
        return makeResult(DbStatus::Ok, "message stored", messageId);
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("sendMessage failed: ") + e.what());
        return makeResult(DbStatus::Error, "database error");
    }
}

std::vector<MessageRecord> Database::getInbox(int userId, int limit)
{
    std::vector<MessageRecord> messages;
    try
    {
        auto conn = pool_.acquire();
        pqxx::read_transaction txn(conn.get());
        const std::string query =
            "SELECT m.id, s.login, r.login, m.text, " +
            timestampExpr("m.created_at") +
            " FROM chat.messages m "
            "JOIN chat.users s ON m.sender_id = s.id "
            "JOIN chat.users r ON m.receiver_id = r.id "
            "WHERE m.receiver_id = $1 ORDER BY m.created_at DESC LIMIT $2";
        const pqxx::result res = txn.exec(pqxx::zview(query.c_str()),
                                          pqxx::params{userId, clampLimit(limit)});
        for (const auto &row : res)
        {
            messages.push_back(MessageRecord{row[0].as<int>(),
                                             row[1].as<std::string>(),
                                             row[2].as<std::string>(),
                                             row[3].as<std::string>(),
                                             row[4].as<std::string>()});
        }
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("getInbox failed: ") + e.what());
    }
    return messages;
}

std::vector<MessageRecord> Database::getHistory(int userId,
                                                const std::string &targetLogin,
                                                int limit,
                                                int offset)
{
    std::vector<MessageRecord> messages;
    try
    {
        auto target = getUserId(targetLogin);
        if (!target)
        {
            return messages;
        }

        auto conn = pool_.acquire();
        pqxx::read_transaction txn(conn.get());
        const std::string query =
            "SELECT m.id, s.login, r.login, m.text, " +
            timestampExpr("m.created_at") +
            " FROM chat.messages m "
            "JOIN chat.users s ON m.sender_id = s.id "
            "JOIN chat.users r ON m.receiver_id = r.id "
            "WHERE (m.sender_id = $1 AND m.receiver_id = $2) "
            "OR (m.sender_id = $2 AND m.receiver_id = $1) "
            "ORDER BY m.created_at ASC LIMIT $3 OFFSET $4";
        const pqxx::result res = txn.exec(pqxx::zview(query.c_str()),
                                          pqxx::params{userId, *target, clampLimit(limit), std::max(0, offset)});
        for (const auto &row : res)
        {
            messages.push_back(MessageRecord{row[0].as<int>(),
                                             row[1].as<std::string>(),
                                             row[2].as<std::string>(),
                                             row[3].as<std::string>(),
                                             row[4].as<std::string>()});
        }
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("getHistory failed: ") + e.what());
    }
    return messages;
}

std::vector<MessageRecord> Database::getAllMessages(int limit)
{
    std::vector<MessageRecord> messages;
    try
    {
        auto conn = pool_.acquire();
        pqxx::read_transaction txn(conn.get());
        const std::string query =
            "SELECT m.id, s.login, r.login, m.text, " +
            timestampExpr("m.created_at") +
            " FROM chat.messages m "
            "JOIN chat.users s ON m.sender_id = s.id "
            "JOIN chat.users r ON m.receiver_id = r.id "
            "ORDER BY m.created_at DESC LIMIT $1";
        const pqxx::result res = txn.exec(pqxx::zview(query.c_str()),
                                          pqxx::params{clampLimit(limit)});
        for (const auto &row : res)
        {
            messages.push_back(MessageRecord{row[0].as<int>(),
                                             row[1].as<std::string>(),
                                             row[2].as<std::string>(),
                                             row[3].as<std::string>(),
                                             row[4].as<std::string>()});
        }
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("getAllMessages failed: ") + e.what());
    }
    return messages;
}

bool Database::hasModeratorRights(int userId)
{
    try
    {
        auto conn = pool_.acquire();
        pqxx::read_transaction txn(conn.get());
        const pqxx::result res = txn.exec(
            pqxx::zview("SELECT role FROM chat.users WHERE id = $1"),
            pqxx::params{userId});
        if (res.empty())
        {
            return false;
        }
        const auto role = res[0][0].as<std::string>();
        return role == "admin" || role == "moderator";
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("hasModeratorRights failed: ") + e.what());
        return false;
    }
}

DbResult Database::banUser(int actorUserId, const std::string &login)
{
    try
    {
        auto conn = pool_.acquire();
        pqxx::work txn(conn.get());
        const pqxx::result res = txn.exec(
            pqxx::zview("UPDATE chat.users SET status = 'banned', updated_at = now() "
                        "WHERE login = $1 RETURNING id"),
            pqxx::params{login});
        if (res.empty())
        {
            return makeResult(DbStatus::NotFound, "user not found");
        }
        audit(txn, actorUserId, "ban", login, "user banned");
        txn.commit();
        Logger::getInstance().warn("user banned: " + login);
        return makeResult(DbStatus::Ok, "user banned", res[0][0].as<int>());
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("banUser failed: ") + e.what());
        return makeResult(DbStatus::Error, "database error");
    }
}

DbResult Database::unbanUser(int actorUserId, const std::string &login)
{
    try
    {
        auto conn = pool_.acquire();
        pqxx::work txn(conn.get());
        const pqxx::result res = txn.exec(
            pqxx::zview("UPDATE chat.users SET status = 'offline', updated_at = now() "
                        "WHERE login = $1 AND status = 'banned' RETURNING id"),
            pqxx::params{login});
        if (res.empty())
        {
            return makeResult(DbStatus::NotFound, "banned user not found");
        }
        audit(txn, actorUserId, "unban", login, "user unbanned");
        txn.commit();
        Logger::getInstance().warn("user unbanned: " + login);
        return makeResult(DbStatus::Ok, "user unbanned", res[0][0].as<int>());
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("unbanUser failed: ") + e.what());
        return makeResult(DbStatus::Error, "database error");
    }
}

DbResult Database::kickUser(int actorUserId, const std::string &login)
{
    try
    {
        auto conn = pool_.acquire();
        pqxx::work txn(conn.get());
        const pqxx::result res = txn.exec(
            pqxx::zview("UPDATE chat.users SET status = 'offline', last_seen = now(), updated_at = now() "
                        "WHERE login = $1 AND status <> 'banned' RETURNING id"),
            pqxx::params{login});
        if (res.empty())
        {
            return makeResult(DbStatus::NotFound, "active non-banned user not found");
        }
        audit(txn, actorUserId, "kick", login, "user session kicked");
        txn.commit();
        Logger::getInstance().warn("user kicked: " + login);
        return makeResult(DbStatus::Ok, "user kicked", res[0][0].as<int>());
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("kickUser failed: ") + e.what());
        return makeResult(DbStatus::Error, "database error");
    }
}

DbResult Database::markUserOffline(const std::string &login)
{
    try
    {
        auto conn = pool_.acquire();
        pqxx::work txn(conn.get());
        const pqxx::result res = txn.exec(
            pqxx::zview("UPDATE chat.users SET status = 'offline', last_seen = now(), updated_at = now() "
                        "WHERE login = $1 AND status <> 'banned' RETURNING id"),
            pqxx::params{login});
        txn.commit();
        if (res.empty())
        {
            return makeResult(DbStatus::NotFound, "user not found or banned");
        }
        return makeResult(DbStatus::Ok, "user offline", res[0][0].as<int>());
    }
    catch (const std::exception &e)
    {
        Logger::getInstance().error(std::string("markUserOffline failed: ") + e.what());
        return makeResult(DbStatus::Error, "database error");
    }
}

void Database::audit(pqxx::work &txn,
                     int actorUserId,
                     const std::string &action,
                     const std::string &targetLogin,
                     const std::string &details)
{
    if (actorUserId <= 0)
    {
        txn.exec(
            pqxx::zview("INSERT INTO chat.audit_log (actor_user_id, action, target_login, details) "
                        "VALUES (NULL, $1, $2, $3)"),
            pqxx::params{action, targetLogin, details});
        return;
    }

    txn.exec(
        pqxx::zview("INSERT INTO chat.audit_log (actor_user_id, action, target_login, details) "
                    "VALUES ($1, $2, $3, $4)"),
        pqxx::params{actorUserId, action, targetLogin, details});
}
