#include "CommandLineInterface.h"
#include "Parsing.h"
#include "Database.h"
#include <iostream>
#include <string>

//'Ανθρωπος

using namespace std;

void CommandLineInterface::parseCommand()
{
	string s;
	if(_currentLoginID != -1)
	{
		cout << "[" << _username << "] ";
		getline(cin, s);
	}
	else getline(cin, s);
	if (startWith("!", s))
	{
		string command = splitBy(" ", s);
		if (command == "!login")
		{
			parseLogin(s);
			if (!s.empty())
			{
				cout << "illegal parameter(s): " << s << endl;
				return;
			}
			if (!callLogin()) return;
			showCountPM();
			showChatMsgs();
		}
		else if (command == "!logout")
		{
			if (!s.empty()) 
			{
				cout << "illegal parameter(s): " << s << endl;
				return;
			}
			if (_currentLoginID == -1) 
			{
				cout << "already logged out" << endl;
				return;
			}
			callLogout();
		}
		else if (command == "!exit")
		{
			if (!s.empty())
			{
				cout << "illegal parameter(s): " << s << endl;
				return;
			}
			callExit();
		}
		else if (command == "!register")
		{
			parseLogin(s);
			if (!s.empty())
			{
				cout << "illegal parameter(s): " << s << endl;
				return;
			}
			if (!callRegister()) return;
			showChatMsgs();
		}
		else if (command == "!pm")
		{
			if (_currentLoginID == -1)
			{
				cout << "Please log in to write to other users!" << endl;
				return;
			}
			if (!parsePM(s)) return;
			callPM(s);
		}
		else if (command == "!getPM")
		{
			if (_currentLoginID == -1)
			{
				cout << "Please log in to receive messages!" << endl;
				return;
			}
			callGetPM();
		}
		else
		{
			cout << "unknown command: " << command << endl;
			return;
		}
	}
	else
	{
		if (_currentLoginID == -1)
		{
			cout << "Please log in to write to the chat!" << endl;
			return;
		}
		if (!s.empty())
		{
			_db.addChatMessage(_username, s);
			cout << "<" << _username << "> : " << s << endl;
		}
	}
}

void CommandLineInterface::parseLogin(string& s)
{
	if (s.empty())
	{
		cout << "enter username" << endl;
		getline(cin, s);
		_username = splitBy(" ", s);
	}
	else
	{
		_username = splitBy(" ", s);
	}
	if (!correctName(_username))
	{
		cout << "incorrect username: " << _username << endl;
		return;
	}
	if (s.empty())
	{
		cout << "enter password" << endl;
		getline(cin, s);
		_password = splitBy(" ", s);
	}
	else
	{
		_password = splitBy(" ", s);
	}
}

bool CommandLineInterface::parsePM(string& s)
{
	if (startWith("(", s))
	{
		skipuntil("(", s);
		string target = splitBy(")", s);
		if (!correctName(target))
		{
			cout << "incorrect user name " << target << endl;
			return false;
		}
		_PMDest = target;
	}
	else if (_PMDest == "")
	{
		string target;
		cout << "enter user name who received message" << endl;
		getline(cin, target);
		if (!correctName(target))
		{
			cout << "incorrect user name " << target << endl;
			return false;
		}
		_PMDest = target;
	}
	return true;
}

bool CommandLineInterface::callRegister()
{
	_PMDest = "";
	_currentLoginID = _db.addUser(_username, _password);
	_password = "";
	if (_currentLoginID < 0) return false;
	return true;
}

bool CommandLineInterface::callLogin()
{
	_PMDest = "";
	_currentLoginID = _db.checkPassword(_username, _password);
	_password = "";
	if (_currentLoginID == -1)
	{
		cout << "Login/password incorrect" << endl;
		return false;
	}
	return true;
}

void CommandLineInterface::callLogout()
{
	_currentLoginID = -1;
	_username = "";
	_PMDest = "";
}

void CommandLineInterface::callExit()
{
	_exit = true;
}

void CommandLineInterface::callPM(string message)
{
	if (_username == _PMDest)
	{
		cout << "self-message" << endl;
	}
	if (!_db.addPrivateMessage(_username, _PMDest, message))
	{
		cout << "user " << _PMDest << " not found" << endl;
		return;
	}
}

void CommandLineInterface::callGetPM()
{
	vector<Message> msgs = _db.getPrivateMessage(_currentLoginID);
	cout << "private messages:" << endl;
	for (auto &m : msgs)
	{
		cout << "<" << m.getSender() << ">: " << m.getText() << endl;
	}
	cout << "---" << endl;
}

void CommandLineInterface::showChatMsgs()
{
	vector<string> chatmsgs = _db.getChatMessages();
	cout << "chat messages:" << endl;
	for (auto &m : chatmsgs)
	{
		cout << m << endl;
	}
	cout << "---" << endl;
}

bool CommandLineInterface::shouldExit()
{
	return _exit;
}

void CommandLineInterface::showCountPM() {
	vector<Message> pm = _db.getPrivateMessage(_currentLoginID);
	int countpm = pm.size();
	if(countpm)	cout << "You have " << pm.size() << " private messages." << endl;
}