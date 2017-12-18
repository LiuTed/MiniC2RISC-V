int getint();
int putchar(int c);
int putint(int i);
int getchar();
long f(int x);
long g(int x) 
{
	long a[60];
	a[0] = 1;
	a[1] = 1;
	int i = 2; 
	while(i < x+1)
	{
		a[i] = a[i - 1] + a[i - 2];
		i = i + 1;
	}
	return a[x];
}/* comment */
int n;
int main() {
	n = getint();
	if (n < 0 || n > 60)
		return 1;
	long c;
	c = f(n);
	putint(c);
	c = 10;
	putchar(c);
	long d;
	d = g(n);
	putint(d);
	d = 10;
	putchar(d);
	return 0;
}
long f(int x) //comment
{
	if (x < 2)
		return 1;
	else
	{
		int a;
		int b;
		a = x - 1;
		b = x - 2;
		return f(a) + f(b); 
	}
}/* comment */