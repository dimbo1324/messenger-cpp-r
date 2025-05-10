#pragma once
#include <string>
#include <iostream>
#include "../../Misc/Flags.h"
#include "ChatOptions.h"
#include "../../Misc/Misc.h"

typedef unsigned int uint;
typedef unsigned long long ullong;

class Message
{
private:
    ullong _id;
    ullong _author_id;
    ullong _recipient_id;
    std::string _text;
    ullong _published;
    msg::status _status;
    Flags<msg::status> flags;

public:
    ~Message() = default;

    /// @brief Создание нового публичного сообщения перед добавлением в базу
    Message(const ullong &author_id, const std::string &text);

    /// @brief Создание публичного сообщения из базы
    Message(const ullong &id, const ullong &author_id, const std::string &text, const ullong &published, const uint status);

    /// @brief Создание нового приватного сообщения для добавления в базу
    Message(const ullong &author_id, const ullong &recipient_id, const std::string &text);

    /// @brief Создание приватного сообщения из базы
    Message(const ullong &id, const ullong &author_id, const ullong &recipient_id, const std::string &text, const ullong &published, const uint status);

    /// @brief Возвращает статус сообщения
    msg::status getStatus();

    uint getStatusInt();

    /// @brief Изменяет статус сообщения
    void setStatus(msg::status status);

    /* добавление/удаление статусов */

    /// @brief проверка на публичное сообщение
    bool isPublic();
    /// @brief Проверка на приватное сообщение
    bool isPrivate();
    /// @brief присваивает статус публичного сообщения
    void toPublic();
    /// @brief присваивает статус приватного сообщения
    void toPrivate();
    /// @brief присваивает статус отправленного сообщения
    void toDelivered();
    /// @brief удаляет статус отправленного сообщения
    void unDelivered();
    /// @brief проверяет статус отправленного сообщения
    bool isDelivered();
    /// @brief присваивает статус "прочитано"
    void read();
    /// @brief удаляет статус "прочитано"
    void toUnread();
    /// @brief проверяет статус "прочитано"
    bool isRead();

    /// @brief получает таймштамп даты публикации
    ullong getPublished();

    /// @brief Добавляет флаг скрытия
    void hide();

    /// @brief Проверяет есть ли в сообщении флаг скрытия
    bool isHidden();

    /// @brief Удаляет флаг скрытия
    void show();

    /// @brief Добавляет флаг публичного сообщения, удаляет приватного
    void to_public();

    /// @brief Добавляет флаг приватного и удаляет флаг публичного сообщения
    void to_private();

    /// @brief Возвращает ID сообщения
    uint getID();

    /// @brief Возвращает ID автора сообщения
    uint getAuthorID();

    /// @brief Возвращает ID получателя приватного сообщения
    uint getRecipientID();

    /// @brief Возвращает текст сообщения
    std::string getText();

    uint getOwnerID();

    /// @brief Выводит в строку данные сообщения для печати
    std::string messageData();
};