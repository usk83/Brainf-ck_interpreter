#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sourceLoad(int argc, char *argv[]);
void codeRun(char code);

char *buffer;

int main(int argc, char *argv[])
{
	int i;
	char code;

	sourceLoad(argc, argv);

	for(i = 0; (code = buffer[i]) != '\0'; i++)
	{
		codeRun(code);
	}
	printf("%i", i);
	puts("");
	return 0;
}

void sourceLoad(int argc, char *argv[])
{
	FILE *fp;
	char *fname;
	long length;

	if(argc == 1)
	{
		puts("You must input a file name.\nUsage: ./bf <source>");
		exit(EXIT_FAILURE);
	}
	else if(argc > 2)
	{
		puts("Too many arguments.\nUsage: ./bf <source>");
		exit(EXIT_FAILURE);
	}

	if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
	{
		puts("Usage: ./bf <source>");
		exit(EXIT_SUCCESS);
	}

	fname = argv[1];
	fp = fopen(fname, "r");
	if(fp == NULL)
	{
		puts("file not found.");
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	/* ファイル全体を格納するメモリを割り当てる */
	buffer = (char*) malloc(length);

	fread(buffer, sizeof(char), length, fp);

	fclose(fp);
}

void codeRun(char code)
{
}