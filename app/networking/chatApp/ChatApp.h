#ifndef CHATAPP_H
#define CHATAPP_H

#include <string>
#include <chrono>
#include <memory>

namespace ChatApp
{

    class User
    {
    private:
        std::string login_;
        std::string password_;
        std::string name_;

    public:
        User(const std::string &login, const std::string &password, const std::string &name);
        const std::string &getLogin() const;
        const std::string &getPassword() const;
        const std::string &getName() const;
    };

    class Message
    {
    private:
        std::string from_;
        std::string to_;
        std::string text_;
        std::chrono::system_clock::time_point timestamp_;

    public:
        Message(const std::string &from, const std::string &to, const std::string &text);
        const std::string &getFrom() const;
        const std::string &getTo() const;
        const std::string &getText() const;
        std::chrono::system_clock::time_point getTimestamp() const;
    };

}

#endif