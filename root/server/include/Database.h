#ifndef DATABASE_H
#define DATABASE_H

#include <pqxx/pqxx>
#include <string>
#include <vector>

class Database
{
public:
    Database(const std::string &connStr);
    ~Database();
    bool registerUser(const std::string &login, const std::string &password);
    bool loginUser(const std::string &login, const std::string &password);
    void sendMessage(int senderId, int receiverId, const std::string &text);
    std::vector<std::string> getInbox(int userId);
    void logEvent(const std::string &eventType, int userId, const std::string &details);

private:
    pqxx::connection *conn_;
};

#endif