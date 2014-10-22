#include <stdio.h>
#include <stdlib.h>

char *buffer;

void sourceLoad(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	// int i;

	sourceLoad(argc, argv);

	printf("%s", buffer);

	return 0;
}

void sourceLoad(int argc, char *argv[])
{
	FILE *sourcefp;
	char *fname;
	long length;

	if(argc == 1)
	{
		puts("please input a file name.");
		exit(EXIT_FAILURE);
	}

	fname = argv[1];
	sourcefp = fopen(fname, "r");
	if(sourcefp == NULL)
	{
		puts("file not found.");
		exit(EXIT_FAILURE);
	}

	fseek(sourcefp, 0, SEEK_END);
	length = ftell(sourcefp);
	fseek(sourcefp, 0L, SEEK_SET);

	/* ファイル全体を格納するメモリを割り当てる */
	buffer = (char*) malloc(length);

	fread(buffer, sizeof(char), length, sourcefp);

	fclose(sourcefp);
}
