#include "Stream.h"

uint Stream::getUint(std::ifstream &stream, uint offset)
{
    uint n_;
    auto pos = stream.tellg();
    pos += offset;
    stream.seekg(pos);
    char _n[4];
    stream.read(_n, 4);
    memcpy(&n_, _n, 4);
    pos -= offset;
    stream.seekg(pos);
    return n_;
}

// void Stream::writeUint(std::ofstream &stream, uint &n, uint offset)
// {
//     auto pos = stream.tellp();
//     stream.seekp(pos += offset);
//     char _n[4];
//     memcpy(_n, &n, 4);
//     stream.write(_n, 4);
//     stream.seekp(pos -= offset);
// }

unsigned long long Stream::getLong64(std::ifstream &stream, uint offset)
{
    unsigned long long n_;
    auto pos = stream.tellg();
    pos += offset;
    stream.seekg(pos);
    char _n[8];
    stream.read(_n, 8);
    memcpy(&n_, _n, 8);
    pos -= offset;
    stream.seekg(pos);
    return n_;
}

std::string Stream::getString(std::ifstream &stream, uint offset)
{
    auto pos = stream.tellg();
    pos += offset;
    stream.seekg(pos);
    const uint size = getUint(stream);
    pos += 4;
    stream.seekg(pos);
    char _s[size];

    stream.read(_s, size);
    pos -= offset + 4;
    stream.seekg(pos);
    std::string s_(_s, size);
    return s_;
}
