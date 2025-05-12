#pragma once
#include <string>
#include <chrono>
namespace ChatApp
{
class Message
{
private:
std::string _from;
std::string _to;
std::string _text;
std::chrono::system_clock::time_point _timestamp;
public:
Message(const std::string &from, const std::string &to, const std::string &text);
const std::string &GetFrom() const;
const std::string &GetTo() const;
const std::string &GetText() const;
std::chrono::system_clock::time_point GetTimestamp() const;
};
}
