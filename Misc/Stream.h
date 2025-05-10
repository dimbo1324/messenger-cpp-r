#pragma once
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
typedef unsigned int uint;
namespace Stream
{
    uint getUint(std::ifstream &stream, uint offset = 0);
    void writeUint(std::ifstream &stream, uint &n, uint offset = 0);
    unsigned long long getLong64(std::ifstream &stream, uint offset = 0);
    std::string getString(std::ifstream &stream, uint offset = 0);

};