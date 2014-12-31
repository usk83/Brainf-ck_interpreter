#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getopt_long/my_getopt.h"
#include "prototype.h"

int main(int argc, char *argv[])
{
	FILE *fp;
	char buffer[BUF_LEN];

	fp = sourceLoad(NULL, buffer, BUF_LEN, argc, argv);

	return EXIT_SUCCESS;
}

FILE *sourceLoad(FILE *fp, char *buffer, int length, int argc, char *argv[])
{
	if (fp == NULL)
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

		printf("args: %d\n", argc-optind);

		// file open to read
		if ((fp = fopen(argv[optind], "r")) == NULL)
		{
			my_strerror(1, argv[0], argv[optind]);
			exit(EXIT_FAILURE);
		}
	}
	return fp;
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
	printf("Usage: %s <source>\n", com);
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
