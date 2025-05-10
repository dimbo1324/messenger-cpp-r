#pragma once
#include <string>
#include "../../Misc/Misc.h"
#include "../../Misc/Flags.h"
#include "../../Misc/SHA1.h"
#include "ChatOptions.h"
typedef unsigned int uint;
typedef unsigned long long ullong;
class User
{
private:
    ullong _id;
    std::string _login;
    std::string _email;
    std::string _first_name;
    std::string _last_name;
    std::string _pass_hash;
    std::string _pass_salt;
    user::status _status = user::none_;
    ullong _registered = 0;
    ullong _session_key = 0;
    Flags<user::status> flags;
    bool _in_db = false;
    SHA1 sha1;

public:
    User(
        const std::string &login,
        const std::string &email,
        const std::string &first_name,
        const std::string &last_name,
        std::string &pass);
    User(
        const std::string &&login,
        const std::string &&email,
        const std::string &&first_name,
        const std::string &&last_name,
        std::string &&pass);
    User(
        const ullong &id,
        const std::string &login,
        const std::string &email,
        const std::string &first_name,
        const std::string &last_name,
        const ullong &registered,
        const uint &status,
        const ullong &session_key,
        const std::string &pass_hash,
        const std::string &pass_salt);
    ~User() = default;
    bool InDB();
    std::string getHash();
    std::string getSalt();
    void setInDB(bool in_db);
    ullong getID();
    std::string getName();
    void setName(const std::string &first_name, const std::string &last_name);
    void setFirstName(const std::string &first_name);
    void setLastName(const std::string &last_name);
    std::string getFirstName();
    std::string getLastName();
    ullong getRegistered();
    std::string getEmail();
    void setEmail(const std::string &email);
    std::string getLogin();
    void setLogin(const std::string &login);
    user::status getStatus();
    void setStatus(user::status &status);
    bool isBanned();
    void unBan();
    void ban();
    bool isAdmin();
    bool isServiceAdmin();
    bool isUser();
    void toAdmin();
    void toUser();
    void setPass(std::string &pass);
    bool validatePass(std::string &pass);
    ullong getSessionKey();
    void setSessionKey(ullong key);
    bool validateSessionKey(ullong key);
    uint getOwnerID();
    std::string userData();
};
