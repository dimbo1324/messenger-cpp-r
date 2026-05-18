#ifndef DATABASE_H
#define DATABASE_H

#include <pqxx/pqxx>

#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <vector>

enum class DbStatus
{
    Ok,
    ValidationError,
    Unauthorized,
    Forbidden,
    NotFound,
    Duplicate,
    Banned,
    Error
};

struct DbResult
{
    DbStatus status{DbStatus::Error};
    std::string message;
    int id{-1};
};

struct LoginResult
{
    DbStatus status{DbStatus::Unauthorized};
    std::string message;
    int userId{-1};
    std::string login;
    std::string role;
};

struct UserSummary
{
    int id{-1};
    std::string login;
    std::string role;
    std::string status;
    std::string lastSeen;
};

struct MessageRecord
{
    int id{-1};
    std::string sender;
    std::string receiver;
    std::string text;
    std::string createdAt;
};

class DatabasePool
{
public:
    class Connection
    {
    public:
        Connection(DatabasePool &pool, std::unique_ptr<pqxx::connection> connection);
        ~Connection();
        Connection(const Connection &) = delete;
        Connection &operator=(const Connection &) = delete;
        Connection(Connection &&other) noexcept;
        Connection &operator=(Connection &&other) noexcept;

        pqxx::connection &get();

    private:
        DatabasePool *pool_;
        std::unique_ptr<pqxx::connection> connection_;
    };

    DatabasePool(std::string connStr, std::size_t poolSize);
    Connection acquire();
    std::size_t size() const;

private:
    friend class Connection;
    void release(std::unique_ptr<pqxx::connection> connection);

    std::string connStr_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::unique_ptr<pqxx::connection>> available_;
};

class Database
{
public:
    Database(const std::string &connStr, std::size_t poolSize);

    bool ping();

    DbResult registerUser(const std::string &login, const std::string &password);
    LoginResult loginUser(const std::string &login, const std::string &password);
    DbResult logoutUser(int userId);

    std::optional<int> getUserId(const std::string &login);
    std::optional<UserSummary> getUserByLogin(const std::string &login);
    std::vector<UserSummary> listUsers();

    DbResult sendMessage(int senderId, const std::string &receiverLogin, const std::string &text);
    std::vector<MessageRecord> getInbox(int userId, int limit);
    std::vector<MessageRecord> getHistory(int userId,
                                          const std::string &targetLogin,
                                          int limit,
                                          int offset);
    std::vector<MessageRecord> getAllMessages(int limit);

    DbResult banUser(int actorUserId, const std::string &login);
    DbResult unbanUser(int actorUserId, const std::string &login);
    DbResult kickUser(int actorUserId, const std::string &login);
    DbResult markUserOffline(const std::string &login);
    bool hasModeratorRights(int userId);

    std::string hashPassword(const std::string &plainPassword);
    bool verifyPassword(const std::string &plainPassword, const std::string &passwordHash);

private:
    std::string hashPassword(pqxx::work &txn, const std::string &plainPassword);
    bool verifyPassword(pqxx::work &txn,
                        const std::string &plainPassword,
                        const std::string &passwordHash);
    void audit(pqxx::work &txn,
               int actorUserId,
               const std::string &action,
               const std::string &targetLogin,
               const std::string &details);

    DatabasePool pool_;
};

#endif
