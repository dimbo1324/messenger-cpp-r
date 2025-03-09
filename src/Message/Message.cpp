#include "Message.h"

namespace ChatApp
{
    Message::Message(const std::string &from, const std::string &to, const std::string &text)
        : _from(from), _to(to), _text(text), _timestamp(std::chrono::system_clock::now()) {}

    const std::string &Message::GetFrom() const
    {
        return _from;
    }

    const std::string &Message::GetTo() const
    {
        return _to;
    }

    const std::string &Message::GetText() const
    {
        return _text;
    }

    std::chrono::system_clock::time_point Message::GetTimestamp() const
    {
        return _timestamp;
    }
}
