int getint();
int putchar(int c);
int putint(int i);
int getchar();
int bubble(int arr[], int n);
int main()
{
	int n;
	n = getint();
	int arr[100];
	int i;
	i=0;
	while(i < n)
	{
		arr[i] = getint();
		i=i+1;
	}
	i=bubble(arr, n);
	i=0;
	int t;
	while(i<n)
	{
		t=arr[i];
		t=putint(t);
		t=32;
		t=putchar(t);
		i=i+1;
	}
	t=10;
	t=putchar(t);
	return 0;
}
int bubble(int arr[], int n)
{
	int i;
	int j;
	i = n - 1;
	while(i>0)
	{
		j=0;
		while(j<i)
		{
			if(arr[j] > arr[j+1])
			{
				int t;
				t=arr[j];
				arr[j]=arr[j+1];
				arr[j+1]=t;
			}
			j=j+1;
		}
		i=i-1;
	}
	return 0;
}