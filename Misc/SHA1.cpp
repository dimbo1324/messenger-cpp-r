#include "SHA1.h"
#define one_block_size_bytes 64
#define one_block_size_uints 16
#define block_expend_size_uints 80
#define SHA1HASHLENGTHBYTES 20
#define SHA1HASHLENGTHUINTS 5
typedef uint *Block;
typedef uint ExpendBlock[block_expend_size_uints];
const uint H[5] = {
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476,
    0xC3D2E1F0};
uint SHA1::cycle_shift_left(uint val, int bit_count)
{
    return (val << bit_count | val >> (32 - bit_count));
}
uint SHA1::bring_to_human_view(uint val)
{
    return ((val & 0x000000FF) << 24) |
           ((val & 0x0000FF00) << 8) |
           ((val & 0x00FF0000) >> 8) |
           ((val & 0xFF000000) >> 24);
}
std::string SHA1::int_to_hex(uint i)
{
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(sizeof(uint) * 2)
           << std::hex << i;
    return stream.str();
}
std::string SHA1::hash(std::string &inStr)
{
    return hash(inStr.data(), inStr.size());
}
std::string SHA1::hash(std::string &&inStr)
{
    return hash(inStr.data(), inStr.size());
}
std::string SHA1::hash(char message[], uint msize_bytes)
{
    uint A = H[0];
    uint B = H[1];
    uint C = H[2];
    uint D = H[3];
    uint E = H[4];
    uint totalBlockCount = msize_bytes / one_block_size_bytes;
    uint needAdditionalBytes =
        one_block_size_bytes - (msize_bytes - totalBlockCount * one_block_size_bytes);
    if (needAdditionalBytes < 8)
    {
        totalBlockCount += 2;
        needAdditionalBytes += one_block_size_bytes;
    }
    else
    {
        totalBlockCount += 1;
    }
    uint extendedMessageSize = msize_bytes + needAdditionalBytes;
    unsigned char *newMessage = new unsigned char[extendedMessageSize];
    memcpy(newMessage, message, msize_bytes);
    newMessage[msize_bytes] = 0x80;
    memset(newMessage + msize_bytes + 1, 0, needAdditionalBytes - 1);
    uint *ptr_to_size = (uint *)(newMessage + extendedMessageSize - 4);
    *ptr_to_size = bring_to_human_view(msize_bytes * 8);
    ExpendBlock exp_block;
    for (int i = 0; i < totalBlockCount; i++)
    {
        unsigned char *cur_p = newMessage + one_block_size_bytes * i;
        Block block = (Block)cur_p;
        for (int j = 0; j < one_block_size_uints; j++)
        {
            exp_block[j] = bring_to_human_view(block[j]);
        }
        for (int j = one_block_size_uints; j < block_expend_size_uints; j++)
        {
            exp_block[j] =
                exp_block[j - 3] ^
                exp_block[j - 8] ^
                exp_block[j - 14] ^
                exp_block[j - 16];
            exp_block[j] = cycle_shift_left(exp_block[j], 1);
        }
        uint a = H[0];
        uint b = H[1];
        uint c = H[2];
        uint d = H[3];
        uint e = H[4];
        for (int j = 0; j < block_expend_size_uints; j++)
        {
            uint f;
            uint k;
            if (j < 20)
            {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            }
            else if (j < 40)
            {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            }
            else if (j < 60)
            {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            }
            else
            {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }
            uint temp = cycle_shift_left(a, 5) + f + e + k + exp_block[j];
            e = d;
            d = c;
            c = cycle_shift_left(b, 30);
            b = a;
            a = temp;
        }
        A = A + a;
        B = B + b;
        C = C + c;
        D = D + d;
        E = E + e;
    }
    delete[] newMessage;
    return int_to_hex(A) + int_to_hex(B) + int_to_hex(C) + int_to_hex(D) + int_to_hex(E);
}
