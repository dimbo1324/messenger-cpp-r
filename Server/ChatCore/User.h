#pragma once
#include <string>
#include "../../Misc/Misc.h"
#include "../../Misc/Flags.h"
#include "../../Misc/SHA1.h"
#include "ChatOptions.h"

typedef unsigned int uint;
typedef unsigned long long ullong;

/// @brief Класс контейнер данных пользователя.
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

    /// @brief Имеется ли пользователь в базе?
    /// @return
    bool InDB();

    /// @brief получает хеш пароля
    std::string getHash();
    /// @brief получает соль
    std::string getSalt();

    /// @brief Обновляет статус принадлежности пользователя к базе.
    void setInDB(bool in_db);

    /// @brief Возвращает ID пользователя
    ullong getID();

    /// @brief Возвращает имя фамилию пользователя
    std::string getName();

    /// @brief Изменяет имя фамилию пользователя. Параметр с пустой строкой игнорируется.
    void setName(const std::string &first_name, const std::string &last_name);
    void setFirstName(const std::string &first_name);
    void setLastName(const std::string &last_name);

    /// @brief получает имя
    std::string getFirstName();
    /// @brief получает фамилию
    std::string getLastName();
    /// @brief получает таймштамп регистрации
    ullong getRegistered();

    /// @brief Возвращает Email
    std::string getEmail();

    /// @brief изменяет email
    void setEmail(const std::string &email);

    /// @brief Возвращает логин пользователя
    std::string getLogin();

    /// @brief Изменяет логин
    void setLogin(const std::string &login);

    /// @brief Получает флаги статуса пользователя
    user::status getStatus();

    /// @brief Меняет флаги статуса пользователя
    void setStatus(user::status &status);

    /// @brief Проверяет флаг блокировки пользователя
    bool isBanned();

    /// @brief Удаляет флаг блокировки пользователя
    void unBan();

    /// @brief Добавляет флаг блокировки пользователя
    void ban();

    /// @brief Проверяет флаг администратора
    bool isAdmin();

    /// @brief Проверяет флаги сервисного администратора
    bool isServiceAdmin();

    /// @brief Проверяет флаг пользователя
    bool isUser();

    /// @brief Устанавливает флаг администратора
    void toAdmin();

    /// @brief Устанавливает флаг пользователя
    void toUser();

    /// @brief Изменяет пароль пользователя
    void setPass(std::string &pass);

    /// @brief проверяет правильность пароля
    bool validatePass(std::string &pass);

    /// @brief получает сессионный ключ
    ullong getSessionKey();

    /// @brief изменяет сессионный ключ
    void setSessionKey(ullong key);

    /// @brief проверяет валидность сессионного ключа
    bool validateSessionKey(ullong key);

    uint getOwnerID();

    /// @brief Записывает в строку все данные пользователя для отображения на экране
    std::string userData();
};
