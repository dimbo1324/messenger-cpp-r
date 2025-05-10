#pragma once
#include <string>
#include <cstring>
#include <iomanip>
typedef unsigned int uint;
class SHA1
{
private:
    uint cycle_shift_left(uint val, int bit_count);
    uint bring_to_human_view(uint val);
    std::string int_to_hex(uint i);

public:
    ~SHA1() = default;
    std::string hash(std::string &inStr);
    std::string hash(std::string &&inStr);
    std::string hash(char message[], uint msize_bytes);
};
