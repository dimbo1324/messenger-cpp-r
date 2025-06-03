#pragma once
#include <string>
using namespace std;
typedef unsigned int uint;

#define one_block_size_bytes 64 // количество байб в блоке
#define one_block_size_uints 16 // количество 4байтовых  в блоке
#define block_expend_size_uints 80 // количество 4байтовых в дополненном блоке

#define SHA1HASHLENGTHBYTES 20
#define SHA1HASHLENGTHUINTS 5

struct Hash
{
	uint c[SHA1HASHLENGTHUINTS]{};
	uint &operator[](int index)
	{
		return c[index];
	}
	bool operator==(const Hash& other) const
	{
		for (int i = 0; i < SHA1HASHLENGTHUINTS; ++i) if (c[i] != other.c[i]) return false;
		return true;
	}
};

typedef uint* Block;
typedef uint ExpendBlock[block_expend_size_uints];

const uint H[SHA1HASHLENGTHUINTS] = {
	0x67452301,
	0xEFCDAB89,
	0x98BADCFE,
	0x10325476,
	0xC3D2E1F0
}; // константы, согласно стандарту

uint cycle_shift_left(uint val, int bit_count);
uint bring_to_human_view(uint val);

Hash sha1(const string &message);
