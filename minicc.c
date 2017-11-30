#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
char buf[65535];
char path[65535];
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
int main(int argc, char** argv)
{
	char* output_filename = strdup("a.t");
	char* input_filename = NULL;
	splitpath(argv[0], path, NULL);
	int argcnt = 1;
	for(argc--, argv++; argc > 0; argc -= argcnt, argv += argcnt)
	{
		argcnt = 1;
		if(!strncmp(argv[0], "-o", 2))
		{
			assert(argc >= 2);
			free(output_filename);
			output_filename = strdup(argv[1]);
			argcnt = 2;
		}
		else
		{
			if(input_filename) free(input_filename);
			input_filename = strdup(argv[0]);
		}
	}
	snprintf(buf, 65534, "cat %s | %s/MiniC2Eeyore/a.out | %s/Eeyore2Tigger/a.out > %s", input_filename, path, path, output_filename);
	system(buf);
	free(output_filename);
	if(input_filename) free(input_filename);
	return 0;
}