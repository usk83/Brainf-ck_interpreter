#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *buffer;
char output[10000]; // 適当
char memory[30000];
short curmem = 15000;

void sourceLoad(int argc, char *argv[]);
void codeRun(char code);

int main(int argc, char *argv[])
{
	int i;
	char code;

	sourceLoad(argc, argv);

	i = 0;
	while((code = buffer[i]) != 0)
	{
		codeRun(code);
		i++;
	}
	puts("");
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

void codeRun(char code)
{
	char temp;
	// char hoge[0];
	switch(code)
	{
		case '+':
				memory[curmem]++;
				break;
		case '-':
				memory[curmem]--;
				break;
		case '>':
				curmem++;
				if(curmem < 0 || curmem > 30000)
				{
					puts("EROOR!");
					exit(EXIT_FAILURE);
				}
				break;
		case '<':
				curmem--;
				if(curmem < 0 || curmem > 30000)
				{
					puts("EROOR!");
					exit(EXIT_FAILURE);
				}
				break;
		case '.':
				putchar(memory[curmem]);
				break;
		case ',':
				printf("\ninput a 1byte character. : ");
				temp = getchar();
				if(temp >= 31 && temp <= 127)
				{
					memory[curmem] = temp;
				}
				else
				{
					puts("EROOR!");
				}
				if(temp != '\n')
				{
					while (getchar() != '\n');
				}
				break;
		case '[':
				break;
		case ']':
				break;
	}
}