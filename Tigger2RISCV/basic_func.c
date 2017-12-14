#include "stdio.h"
int putint(int n)
{
	printf("%d", n);
}
int getint()
{
	int n;
	scanf("%d", &n);
	return n;
}