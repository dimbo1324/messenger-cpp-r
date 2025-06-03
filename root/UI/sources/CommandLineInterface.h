#pragma once
#include <string>
#include "Database.h"
using namespace std;

class CommandLineInterface
{
	int _currentLoginID = -1;
	string _username = "";
	string _password;
	string _PMDest = "";
	Database _db;
	bool _exit = false;
	void parseLogin(string&);
	bool parsePM(string&);
	bool callRegister();
	bool callLogin();
	void callLogout();
	void callExit();
	void callPM(string);
	void callGetPM();
	void showChatMsgs();
	void showCountPM();
public:
	void parseCommand();
	bool shouldExit();
};