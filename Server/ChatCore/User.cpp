#include "User.h"

User::User(const std::string &login, const std::string &email, const std::string &first_name, const std::string &last_name, std::string &pass)
    : _login(login),
      _email(email),
      _first_name(first_name),
      _last_name(last_name),
      _registered(time(NULL)),
      _status(user::user_),
      _in_db(false)
{
    setPass(pass);
}

User::User(const std::string &&login, const std::string &&email, const std::string &&first_name, const std::string &&last_name, std::string &&pass)
    : _login(login),
      _email(email),
      _first_name(first_name),
      _last_name(last_name),
      _registered(time(NULL)),
      _status(user::user_),
      _in_db(false)
{
    setPass(pass);
}

User::User(const ullong &id,
           const std::string &login,
           const std::string &email,
           const std::string &first_name,
           const std::string &last_name,
           const ullong &registered,
           const uint &status,
           const ullong &session_key,
           const std::string &pass_hash,
           const std::string &pass_salt)
    : _id(id),
      _login(login),
      _email(email),
      _first_name(first_name),
      _last_name(last_name),
      _registered(registered),
      _status((user::status)status),
      _session_key(session_key),
      _pass_hash(pass_hash),
      _pass_salt(pass_salt),
      _in_db(true) {}

bool User::InDB()
{
    return _in_db;
}

std::string User::getHash()
{
    return _pass_hash;
}

std::string User::getSalt()
{
    return _pass_salt;
}

void User::setInDB(bool in_db)
{
    _in_db = in_db;
}

ullong User::getID()
{
    return _id;
}

std::string User::getName()
{
    return _first_name + " " + _last_name;
}

void User::setName(const std::string &first_name, const std::string &last_name)
{
    if (!first_name.empty())
    {
        _first_name = first_name;
    }
    if (!last_name.empty())
    {
        _last_name = last_name;
    }
}

void User::setFirstName(const std::string &first_name)
{
    _first_name = first_name;
}

void User::setLastName(const std::string &last_name)
{
    _last_name = last_name;
}

std::string User::getFirstName()
{
    return _first_name;
}

std::string User::getLastName()
{
    return _last_name;
}

ullong User::getRegistered()
{
    return _registered;
}

std::string User::getEmail()
{
    return _email;
}

void User::setEmail(const std::string &email)
{
    _email = email;
}

std::string User::getLogin()
{
    return _login;
}
void User::setLogin(const std::string &login)
{
    _login = login;
}
user::status User::getStatus()
{
    return _status;
}
void User::setStatus(user::status &status)
{
    if (getID() == 1)
    {
        return;
    }
    _status = status;
}

void User::toUser()
{
    if (getID() == 1)
    {
        return;
    }
    _status = flags.flagsReplace(_status, user::status::user_, user::status::admin_);
}

bool User::isBanned()
{
    return flags.hasFlag(_status, user::banned_);
}
void User::unBan()
{
    _status = flags.removeFlag(_status, user::banned_);
}
void User::ban()
{
    if (getID() == 1)
    {
        return;
    }
    toUser();
    _status = flags.addFlag(_status, user::banned_);
}

bool User::isAdmin()
{
    return flags.hasFlag(_status, user::admin_);
}
bool User::isServiceAdmin()
{
    return flags.hasFlag(_status, user::admin_) && getID() == 1;
}
bool User::isUser()
{
    return flags.hasFlag(_status, user::user_);
}
void User::toAdmin()
{
    _status = flags.flagsReplace(_status, user::admin_, user::user_ | user::banned_);
}

void User::setPass(std::string &pass)
{
    _pass_salt = Misc::getRandomStr(40);
    _pass_hash = sha1.hash(pass + _pass_salt);
    // уничтожение пароля
    for (int i{0}; i < pass.size(); i++)
        pass.data()[i] = '\0';
}

bool User::validatePass(std::string &pass)
{
    std::string pass_hash = sha1.hash(pass + _pass_salt);
    // уничтожение пароля
    for (int i{0}; i < pass.size(); i++)
        pass.data()[i] = '\0';
    return pass_hash == _pass_hash;
}

ullong User::getSessionKey()
{
    return _session_key;
}

void User::setSessionKey(ullong key)
{
    _session_key = key;
}

bool User::validateSessionKey(ullong key)
{
    if (_session_key == 0)
        return false;
    return key == _session_key;
}

uint User::getOwnerID()
{
    return getID();
}

std::string User::userData()
{
    if (getID() == 0)
    {
        return "Пользователь удален";
    }
    std::string s1 =
        "[userid " + std::to_string(_id) + "] [" + _login + "] [" + getName() + "] [" + getEmail() + "] ";

    if (isAdmin())
        s1 += "[group admin] ";
    if (isBanned())
        s1 += "[status banned] ";
    if (isUser())
        s1 += "[group user] ";
    s1 += "Рег." + Misc::StampToTime(_registered);
    return s1;
}
