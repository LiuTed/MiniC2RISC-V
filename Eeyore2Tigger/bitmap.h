#ifndef BITMAP_H
#define BITMAP_H
#include <cstdio>
class bitmap
{
	char *bits;
	int len, bytes;
	static const int BIC = (sizeof(char)<<3);
public:
	bitmap();
	bitmap(int);
	bitmap(const bitmap&);
	~bitmap();
	void set(int);
	void clear(int);
	void reset();
	int test(int) const;
	int length() const;
	int find() const;
	bitmap& operator = (const bitmap&);
	bitmap& operator &= (const bitmap&);
	bitmap& operator |= (const bitmap&);
	bool operator == (const bitmap&) const;
	void print()
	{
		for(int i = len - 1; i >= 0; i--)
		{
			std::printf("%d",test(i));
		}
	}
};
#endif