#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getopt_long/my_getopt.h"
#include "prototype.h"

#define DBG(...) (printf("%s %u @%s(): ",__FILE__,__LINE__,__func__), printf(__VA_ARGS__)), puts("")
#define BUF_LEN 1024

typedef unsigned char BYTE;

typedef struct memory
{
	BYTE value = 0;
	struct memory* next;
	struct memory* prev;
} MEMORY;

int main(int argc, char *argv[])
{
	FILE *fp = NULL;
	char buffer[BUF_LEN];
	int i;

	source_load(&fp, buffer, sizeof(buffer), argc, argv);

	while (1)
	{
		for (i=0; buffer[i] != '\0'; i++)
		{
			// ソースファイルを一文字ずつ表示できることを確認
			printf("buffer[%d]:%c\n", i, buffer[i]);
			// code_run(buffer[i]);
		}

		// fpが終端に達していなければ追加で読み込み
		if (!feof(fp))
		{
			source_load(&fp, buffer, sizeof(buffer), argc, argv);
		}
		// 終端に達していたらファイルの見込みを終了
		else
		{
			if (fclose(fp) == EOF)
			{
				my_strerror(2, argv[0], argv[optind]);
				exit(EXIT_FAILURE);
			}
			break;
		}
	}

	return EXIT_SUCCESS;
}

void code_run(char code)
{
	static MEMORY *header = (MEMORY*) malloc(sizeof(MEMORY));
	// switch分コピーから再開
}

void source_load(FILE **fp, char *buffer, int length, int argc, char *argv[])
{
	int size; // freadの戻り値を格納
	if (*fp == NULL)
	{
		// 引数がないとき
		if(argc == 1)
		{
			my_strerror(13, argv[0], NULL);
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}

		// オプションの処理
		checkopt(argc, argv);

		// オプション関係以外に引数がないとき
		if (argc == optind)
		{
			my_strerror(13, argv[0], NULL);
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
		// オプション以外の引数が2つ以上のとき
		else if (argc - optind > 1)
		{
			my_strerror(12, argv[0], NULL);
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}

		// file open to read
		if ((*fp = fopen(argv[optind], "r")) == NULL)
		{
			my_strerror(1, argv[0], argv[optind]);
			exit(EXIT_FAILURE);
		}
	}
	size = fread(buffer, sizeof(char), length-1, *fp);
	if (size  == EOF && !feof(*fp))
	{
		my_strerror(3, argv[0], argv[optind]);
		exit(EXIT_FAILURE);
	}
	buffer[size] = '\0';
}

void my_strerror(short errcode, const char *com, const char *option)
{
	// ~100: error
	if (errcode < 100)
	{
		fprintf(stderr, "%s: error: ", com);
	}
	// 100~: warnig
	else
	{
		fprintf(stderr, "%s: warning: ", com);
	}

	switch (errcode)
	{
		case 1:
			fprintf(stderr, "no such file or directory \"%s\".\n", option);
			break;
		case 2:
			fprintf(stderr, "failed to close \"%s\".\n", option);
			break;
		case 3:
			fprintf(stderr, "failed to read \"%s\".\n", option);
			break;
		// 10~: arguments error
		case 11:
			fprintf(stderr, "too little arguments.\n\n");
			break;
		case 12:
			fprintf(stderr, "too many arguments.\n\n");
			break;
		case 13:
			fprintf(stderr, "missing source filename.\n\n");
			break;
		// 100~: option warning
		case 101:
			fprintf(stderr, "invalid option -%s skipped.\n", option);
			break;
		case 102:
			fprintf(stderr, "invalid option --%s skipped.\n", option);
			break;
		default:
			fprintf(stderr, "undefined error.\n");
			break;
	}
}

void usage(const char *com)
{
	printf("usage: %s <source>\n", com);
}

void checkopt(int argc, char *argv[])
{
	int opt, option_index, long_opt;
	int i, j;

	// helpオプションがある場合はusageのみ表示されるようにする
	for (i=0; i<argc; i++)
	{
		if (!strcmp(argv[i], "--help"))
		{
			opterr = 0; // エラーメッセージを非表示にする
		}
		else if (argv[i][0] == '-')
		{
			for (j=1; j<strlen(argv[i]); j++)
			{
				if (argv[i][j] == 'h')
				{
					opterr = 0; // エラーメッセージを非表示にする
				}
			}
		}
		}

	struct option long_options[] =
	{
		{"help", no_argument, &long_opt, 'h'},
		{0, 0, 0, 0}
	};

	while((opt = my_getopt_long(argc, argv, "h", long_options, &option_index)) != -1)
	{
		switch(opt)
		{
			case 0:
				switch (long_opt)
				{
					case 'h':
						usage(argv[0]);
						exit(EXIT_SUCCESS);
						break;
				}
				break;
			case 'h':
				usage(argv[0]);
				exit(EXIT_SUCCESS);
				break;
			case '?':
				// Unknown or required argument.
				break;
		}
	}

	return;
}
