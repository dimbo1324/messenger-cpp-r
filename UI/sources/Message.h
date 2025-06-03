#pragma once
#include <string>

class Message
{
	static int num;		//уникальный номер, который получает id
	static int messageCounter;
	int _id;
	std::string _sender;
	int _destID;
	std::string _text;
public:
	Message();
	Message(std::string writer, std::string text);//сообщение в чат
	Message(std::string writer, int target, std::string text);//личное сообщение

  bool searchByTarget(int) const;
  std::string getSender() const;
  int getDest() const;
  int getID() const;
  std::string getText() const;

};
