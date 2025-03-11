#include "ChatApp.h"

namespace ChatApp
{

    User::User(const std::string &login, const std::string &password, const std::string &name)
        : login_(login), password_(password), name_(name) {}

    const std::string &User::getLogin() const
    {
        return login_;
    }

    const std::string &User::getPassword() const
    {
        return password_;
    }

    const std::string &User::getName() const
    {
        return name_;
    }

    Message::Message(const std::string &from, const std::string &to, const std::string &text)
        : from_(from), to_(to), text_(text), timestamp_(std::chrono::system_clock::now()) {}

    const std::string &Message::getFrom() const
    {
        return from_;
    }

    const std::string &Message::getTo() const
    {
        return to_;
    }

    const std::string &Message::getText() const
    {
        return text_;
    }

    std::chrono::system_clock::time_point Message::getTimestamp() const
    {
        return timestamp_;
    }

}