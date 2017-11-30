#include "bitmap.h"
#include <cstring>
#include <algorithm>
using namespace std;

static int divRoundUp(int a, int b)
{
	int k = a / b;
	if(k * b >= a) return k;
	return k + 1;
}

bitmap::bitmap()
{
	len=bytes=0;
	bits=nullptr;
}
bitmap::bitmap(int n)
{
	len = n;
	bytes = divRoundUp(len, BIC);
	bits = new char[bytes];
	memset(bits, 0, bytes*sizeof(char));
}
bitmap::bitmap(const bitmap& b)
{
	len = b.len;
	bytes = divRoundUp(len, BIC);
	bits = new char[bytes];
	memmove(bits, b.bits, bytes*sizeof(char));
}
bitmap::~bitmap()
{
	delete[] bits;
}
void bitmap::set(int n)
{
	if(n >= len || n < 0) return;
	bits[n / BIC] |= 1 << (n%BIC);
}
void bitmap::clear(int n)
{
	if(n >= len || n < 0) return;
	bits[n / BIC] &= ~(1 << (n%BIC));
}
void bitmap::reset()
{
	for(int i = 0; i < bytes; i++)
		bits[i] = 0;
}
int bitmap::test(int n) const
{
	if(n >= len || n < 0) return -1;
	return (bits[n / BIC] >> (n%BIC)) & 1;
}
int bitmap::length() const
{
	return len;
}
int bitmap::find() const
{
	for(int i = 0; i < len; i++)
		if(!(bits[i/BIC] & (1 << (i%BIC))))
			return i;
	return -1;
}
bitmap& bitmap::operator = (const bitmap& b)
{
	delete[] bits;
	bytes = b.bytes;
	len = b.len;
	bits = new char[bytes];
	memmove(bits, b.bits, bytes*sizeof(char));
}
bitmap& bitmap::operator &= (const bitmap& b)
{
	int j = min(bytes, b.bytes);
	for(int i = 0; i < j; i++)
		bits[i] &= b.bits[i];
}
bitmap& bitmap::operator |= (const bitmap& b)
{
	int j = min(bytes, b.bytes);
	for(int i = 0; i < j; i++)
		bits[i] |= b.bits[i];
}
bool bitmap::operator == (const bitmap& b) const
{
	if(len != b.len) return false;
	for(int i = 0; i < bytes; i++)
	{
		if(bits[i] != b.bits[i]) return false;
	}
	return true;
}
