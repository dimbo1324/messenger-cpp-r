#pragma once
#include <iostream>
#include <string>
#include <codecvt>
#include <cstring>
#include <locale>
#include <time.h>
#include <vector>
#include <fstream>
typedef unsigned int uint;
typedef unsigned long long ullong;
namespace Misc
{
    void printMessage(const std::string &msg, bool endl = true);
    std::wstring string_to_wstring(const std::string &str);
    std::string wstring_to_string(const std::wstring &wstr);
    uint getSymbolsCount(const std::string string);
    std::string StampToTime(long long timestamp);
    uint getInt(char buffer[], uint offset = 0);
    unsigned long long getLong(char buffer[], uint offset = 0);
    std::string getString(char buffer[], uint offset = 0);
    std::string getString(const char buffer[], uint len, uint offset);
    void writeIntBuffer(uint num, char buffer[], uint offset = 0);
    void writeUlongBuffer(unsigned long long num, char buffer[], uint offset = 0);
    void writeStringBuffer(std::string &str, char buffer[], uint offset = 0, bool add_size = true, uint max_size = 4092);
    void writeStringBuffer(std::string &&str, char buffer[], uint offset = 0, bool add_size = true, uint max_size = 4092);
    uint findDynamicData(char buffer[], uint offset = 0, uint offset_data = 0, uint max_size = 1024);
    std::vector<char> writeVectorBuffer(char buffer[], uint len);
    void writeVectorBuffer(char buffer[], const std::vector<char> &vector);
    std::vector<char> writeVectorBuffer(const std::string &str);
    ullong getRandomKey();
    std::vector<std::string> stringExplode(std::string const &str, const std::string &delimeter);
    std::vector<std::string> stringExplode(std::string const &str, char delimeter);
    std::string ltrimString(std::string &string, uint max);
    std::string getRandomStr(const unsigned char length);
    std::string getConfigValue(const std::string &path, const std::string &section, const std::string &value);
    void alignPaginator(ullong &start, ullong &per_page, const ullong &count);
}