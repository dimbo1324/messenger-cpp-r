#include <string>
#include "Parsing.h"

using namespace std;

bool startWith(string tmpl, string& s)
{
	if (tmpl.length() > s.length()) return false;
	return !s.compare(0, tmpl.length(), tmpl);
}

bool startWith(string tmpl, const char* s)
{
	if (tmpl.empty()) return false;
	//проверяем равенство каждого символа шаблона с символами строки
	for (int iptr = 0; iptr < tmpl.length(); ++iptr)
	{
		if (s[iptr] == '\0') return false;			//достигнут конец строки
		if (tmpl[iptr] != s[iptr]) return false;	//есть различия
	}
	return true;//все символы прошли тест на равенство - значит различий нет
}

int findFirst(string tmpl, string& s)
{
	const char* ss = s.c_str();
	int index = -1;
	//ищем позицию начиная с которой встречается шаблон
	for (int shift = 0; shift < (s.length() - tmpl.length() + 1); ++shift)
	{
		//чтобы не создавать копии подстрок после чего проводить сравнения - просто перемещаем указатель
		if (startWith(tmpl, ss + shift))
		{
			index = shift;
			break;
		}
	}
	return index;
}

string splitBy(string tmpl, string& s)
{
	int index = findFirst(tmpl, s);
	if (index == -1)//шаблона нет - перемещаем всё в вывод, очищаем исходную строку.
	{
		string temp = s;
		s.clear();
		return temp;
	}
	string temp = s.substr(0, index);
	s.erase(0, index + tmpl.length());
	return temp;
}

void skipuntil(string tmpl, string& s)
{
	int index = findFirst(tmpl, s);
	if (index == -1) s.clear();
	s.erase(0, index + tmpl.length());
}

string segment(string from, string to, string s)
{
	skipuntil(from, s);
	return splitBy(to, s);
}

bool isDigit(char c)
{
	return (c >= '0') && (c <= '9');
}

bool isLetter(char c)
{
	return (c >= 'a') && (c <= 'z') || (c >= 'A') && (c <= 'Z');
}


bool correctName(string s)
{
	if (s.empty()) return false;
	if (!isLetter(s[0])) return false;
	for (string::iterator it = s.begin(); it != s.end(); ++it)
	{
		char c = (*it);
		if ((!isDigit(c)) && (!isLetter(c)) && (c != '_')) return false;
	}
	return true;
}