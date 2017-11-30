int getint();
int putchar(int c);
int putint(int i);
int getchar();
int f(int x);
int g(int x) 
{
	int a[40];
	a[0] = 1;
	a[1] = 1;
	int i;
	i = 2; 
	while(i<x+1)
	{
		a[i] = a[i - 1] + a[i - 2];
		i = i + 1;
	}
	return a[x];
}/* comment */
int n;
int main() {
	n = getint();
	if (n < 0 || n > 30)
		return 1;
	int c;
	int result;
	c = f(n);
	result = putint(c);
	c = 10;
	result = putchar(c);
	int d;
	d = g(n);
	result = putint(d);
	d = 10;
	result = putchar(d);
	return 0;
}
int f(int x) //comment
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