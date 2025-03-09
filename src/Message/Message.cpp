#include "Message.h"
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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

    std::string Message::serialize() const
    {
        json j;
        j["from"] = _from;
        j["to"] = _to;
        j["text"] = _text;
        j["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
                             _timestamp.time_since_epoch())
                             .count();
        return j.dump();
    }

    Message Message::deserialize(const std::string &jsonStr)
    {
        json j = json::parse(jsonStr);
        std::string from = j["from"];
        std::string to = j["to"];
        std::string text = j["text"];
        auto timestampSeconds = j["timestamp"].get<long long>();
        std::chrono::system_clock::time_point timestamp{std::chrono::seconds{timestampSeconds}};
        Message msg(from, to, text);
        msg._timestamp = timestamp;
        return msg;
    }
}