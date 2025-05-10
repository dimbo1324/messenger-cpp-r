#include "Message.h"

Message::Message(const ullong &author_id, const std::string &text)
    : _author_id(author_id),
      _text(text),
      _status(msg::public_),
      _published(time(NULL)) {}

Message::Message(const ullong &id, const ullong &author_id, const std::string &text, const ullong &published, const uint status)
    : _id(id),
      _author_id(author_id),
      _text(text),
      _status((msg::status)status),
      _published(published) {}

Message::Message(const ullong &author_id, const ullong &recipient_id, const std::string &text)
    : _author_id(author_id),
      _recipient_id(recipient_id),
      _text(text),
      _status(msg::private_),
      _published(time(NULL)) {}

Message::Message(const ullong &id, const ullong &author_id, const ullong &recipient_id, const std::string &text, const ullong &published, const uint status)
    : _id(id),
      _author_id(author_id),
      _recipient_id(recipient_id),
      _text(text),
      _status((msg::status)status),
      _published(published) {}

msg::status Message::getStatus()
{
    return _status;
}

uint Message::getStatusInt()
{
    return (uint)_status;
}

void Message::setStatus(msg::status status)
{
    _status = status;
}

bool Message::isPublic()
{
    return flags.hasFlag(msg::public_, getStatus());
}

bool Message::isPrivate()
{
    return flags.hasFlag(msg::private_, getStatus());
}

void Message::toPublic()
{
    flags.removeFlag(msg::private_, _status);
    flags.addFlag(msg::public_, _status);
}

void Message::toPrivate()
{
    flags.removeFlag(msg::public_, _status);
    flags.addFlag(msg::private_, _status);
}

void Message::toDelivered()
{
    flags.addFlag(msg::message_delivered, _status);
}

void Message::unDelivered()
{
    flags.removeFlag(msg::message_delivered, _status);
}

bool Message::isDelivered()
{
    return flags.hasFlag(msg::message_delivered, _status);
}

void Message::read()
{
    flags.addFlag(msg::message_read, _status);
}

void Message::toUnread()
{
    flags.removeFlag(msg::message_read, _status);
}

bool Message::isRead()
{
    return flags.hasFlag(msg::message_read, _status);
}

ullong Message::getPublished()
{
    return _published;
}

void Message::hide()
{
    _status = flags.addFlag(_status, msg::hidden_);
}

bool Message::isHidden()
{
    return flags.hasFlag(_status, msg::hidden_);
}

void Message::show()
{
    _status = flags.removeFlag(_status, msg::hidden_);
}

void Message::to_public()
{
    _status = flags.flagsReplace(_status, msg::public_, msg::private_);
}

void Message::to_private()
{
    _status = flags.flagsReplace(_status, msg::private_, msg::public_);
}

uint Message::getID()
{
    return _id;
}

uint Message::getAuthorID()
{
    return _author_id;
}

uint Message::getRecipientID()
{
    return _recipient_id;
}

std::string Message::getText()
{
    if (isHidden())
        return "Текст сообщения скрыт администратором";
    return _text;
}

uint Message::getOwnerID()
{
    return _author_id;
}

std::string Message::messageData()
{
    std::string s1;
    s1 += "[msgid " + std::to_string(getID()) + "] ";
    s1 += "Дата публикации: " + Misc::StampToTime(getPublished()) + "\n";
    s1 += "Статус: ";
    if (isPrivate())
        s1 += "[личное] ";
    if (isPublic())
        s1 += "[публичное] ";
    if (isHidden())
        s1 += "[скрыто] ";
    if (isRead())
        s1 += "[прочитано] ";
    if (!isRead())
        s1 += "[не прочитано] ";
    if (isDelivered())
        s1 += "[доставлено] ";

    s1 += "\nТекст: " + getText();
    return s1;
}
