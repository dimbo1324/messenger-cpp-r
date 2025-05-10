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
    Message(const ullong &author_id, const std::string &text);
    Message(const ullong &id, const ullong &author_id, const std::string &text, const ullong &published, const uint status);
    Message(const ullong &author_id, const ullong &recipient_id, const std::string &text);
    Message(const ullong &id, const ullong &author_id, const ullong &recipient_id, const std::string &text, const ullong &published, const uint status);
    msg::status getStatus();
    uint getStatusInt();
    void setStatus(msg::status status);
    bool isPublic();
    bool isPrivate();
    void toPublic();
    void toPrivate();
    void toDelivered();
    void unDelivered();
    bool isDelivered();
    void read();
    void toUnread();
    bool isRead();
    ullong getPublished();
    void hide();
    bool isHidden();
    void show();
    void to_public();
    void to_private();
    uint getID();
    uint getAuthorID();
    uint getRecipientID();
    std::string getText();
    uint getOwnerID();
    std::string messageData();
};