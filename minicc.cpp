#include <cstring>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <assert.h>
#include <unistd.h>
using namespace std;
char path[65535];
char buf[65535];
static void splitpath(const char* full, char* path, char* name)
{
	int t = 0, i = 0;
	while(full[i])
	{
		if(full[i] == '/') t = i;
		i++;
	}
	if(path)
	{
		memmove(path, full, t*sizeof(char));
		path[t] = '\0';
	}
	if(name)
	{
		memmove(name, full+t+1, (i-t)*sizeof(char));
	}
}
void usage(const char* name)
{
	printf("usage: %s input_filename [-o output_filename] [-e/t/s]\n", name);
	exit(0);
}
int main(int argc, char** argv)
{
	const char* output_filename = NULL;
	const char* input_filename = NULL;
	splitpath(argv[0], path, NULL);
	char a;
	int to = 2, from = 0;
	string command;
	while((a=getopt(argc, argv, "o:etsh"))!=-1)
	{
		switch(a)
		{
			case 'o':
			assert(optarg);
			output_filename = optarg;
			break;
			case 'e': to = 0; break;
			case 't': to = 1; break;
			case 's': to = 2; break;
			case 'h':
			default: usage(argv[0]); break;
		}
	}
	if(!output_filename)
	{
		switch(to)
		{
			case 0: output_filename="a.e";break;
			case 1: output_filename="a.t";break;
			case 2: output_filename="a.s";break;	
		}
	}
	input_filename = argv[optind];
	if(!input_filename)
	{
		usage(argv[0]);
	}
	int len = strlen(input_filename);
	switch(input_filename[len-1])
	{
		case 'c': from = 0; break;
		case 'e': from = 1; break;
		case 't': from = 2; break;
		default: break;
	}
	snprintf(buf, 65534, "cat %s ", input_filename);
	command = buf;
	for(int i = from; i <= to; i++)
	{
		switch(i)
		{
			case 0:
				snprintf(buf, 65534, "| %s/MiniC2Eeyore/a.out ", path);
				command += buf;
				break;
			case 1:
				snprintf(buf, 65534, "| %s/Eeyore2Tigger/a.out ", path);
				command += buf;
				break;
			case 2:
				snprintf(buf, 65534, "| %s/Tigger2RISCV/a.out ", path);
				command += buf;
				break;
		}
	}
	snprintf(buf, 65534, "> %s\n", output_filename);
	command += buf;
	system(command.c_str());
	return 0;
}