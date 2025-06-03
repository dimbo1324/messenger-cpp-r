#include "Message.h"
int Message::num = 0;
int Message::messageCounter = 0;
//сообщение в чат
Message::Message() : _id(num), _sender(""), _destID(0), _text("") 
{
} 
Message::Message(std::string writer, std::string text) : _id(++num), _sender(writer), _destID(-1), _text(text)
{   
    messageCounter++;
}
//личное сообщение
Message::Message(std::string writer, int target, std::string text) : _id(++num), _sender(writer), _destID(target), _text(text)
{
    messageCounter++;
}

bool Message::searchByTarget(int n) const {
    return (_destID == n); 
}

std::string Message::getSender() const {
    return _sender;
}

int Message::getDest() const {
    return _destID;
}

std::string Message::getText() const {
    return _text;
}

int Message::getID() const {
    return _id;
}
