#pragma once
#include <vector>
#include "User.h"
#include "Message.h"
#include <string>
#include <unordered_map>

class Database
{
	vector<User> _users;
	vector<Message> _messages;
	unordered_map<string, int> _usersMapByName;
	int searchUserByName(string);
public:
  vector<string> getUserList() const;
  string getUserName(int userId) const;
	Database();
	int addUser(string username, string password);
	int checkPassword(string username, string password);
	void addChatMessage(string sender, string);
	bool addPrivateMessage(string sender, string target, string message);
	vector<string> getChatMessages();//показать все сообщения
  vector<Message> getPrivateMessage(int userID = -1);//показать личные сообщения пользователю username
};
