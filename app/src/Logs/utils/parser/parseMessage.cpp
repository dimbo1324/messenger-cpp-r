#include <string>
#include "../structMessage.h"

Message parseMessage(const std::string &line)
{
    Message msg;
    size_t fromStart = line.find("From: ") + 6;
    size_t fromEnd = line.find(", To: ");
    msg.from = line.substr(fromStart, fromEnd - fromStart);
    size_t toStart = fromEnd + 6;
    size_t toEnd = line.find(", Text: ");
    msg.to = line.substr(toStart, toEnd - toStart);
    size_t textStart = toEnd + 8;
    size_t textEnd = line.find(", Timestamp: ");
    msg.text = line.substr(textStart, textEnd - textStart);
    size_t timestampStart = textEnd + 13;
    msg.timestamp = line.substr(timestampStart);
    return msg;
}