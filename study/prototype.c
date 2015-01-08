#include "prototype.h"

#define DBG(...) (printf("%s %u @%s(): ",__FILE__,__LINE__,__func__), printf(__VA_ARGS__)), puts("")

int main(int argc, char *argv[])
{
	FILE *fp = NULL;
	BUFFER bf_buffer;
	int i;

	const char *source = source_load(&fp, &bf_buffer, sizeof(bf_buffer.value), argc, argv);

	while (1)
	{
		for (i=0; bf_buffer.value[i] != '\0'; i++)
		{
			// debug
			printf("dbg point %d\n", i);
			// if (i == 134) exit(EXIT_FAILURE);

			if (code_run(&bf_buffer, &i) == ERROR)
			{
				puts("");
				my_strerror(91, source, NULL);
				exit(EXIT_FAILURE);
			}
		}

		// fpが終端に達していなければ追加で読み込み
		if (!feof(fp))
		{
			source_load(&fp, &bf_buffer, sizeof(bf_buffer.value), argc, argv);
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

BF_OPERATOR code_run(BUFFER *bf_buffer, int *index)
{
	static MEMORY *bf_memory;
	static short header;
	static int loop = 0;
	char input;
	int start;

	// debug
	// DBG("%d\n", *index);
	// if (*index == 246)
	// 	printf("%d\n", bf_memory->cell[header]);

	if (bf_memory == NULL)
	{
		bf_memory = (MEMORY*) malloc(sizeof(MEMORY));
		header = sizeof(bf_memory->cell)/2;
		memory_init(bf_memory);
	}
	switch(bf_buffer->value[*index])
	{
		case '+':
			bf_memory->cell[header]++;
			return PLUS;
			break;
		case '-':
			bf_memory->cell[header]--;

			// debug
			// if (*index == 214)
				// printf("cell: %d header: %d\n", bf_memory->cell[header], header);
			// if (*index == 171)
			// {
			// 	printf("マイナス後 %d\n", bf_memory->cell[header]);
			// }
			// printf("%d\n", bf_memory->cell[header]);

			return MINUS;
			break;
		case '>':
			if (header == 29)
			{
				// 次がなかったら作成
				if (bf_memory->next == NULL)
					bf_memory = memory_new(bf_memory, NEXT);
				else
					bf_memory = bf_memory->next;
				header = 0;
			}
			else
				header++;

			return NEXT;
			break;
		case '<':
			if (header == 0)
			{
				// 前がなかったら作成
				if (bf_memory->prev == NULL)
					bf_memory = memory_new(bf_memory, PREV);
				else
					bf_memory = bf_memory->prev;
				header = 29;
			}
			else
				header--;

			return PREV;
			break;
		case ',':
			printf("\ninput a 1byte character: ");
			input = getchar();
			bf_memory->cell[header] = input;
			// 二文字目以降があるときは捨てる
			if(input != '\n')
				while (getchar() != '\n');

			return INPUT;
			break;
		case '.':
			// 表示に問題ない文字は表示
			if (code_check(bf_memory->cell[header]))
				putchar(bf_memory->cell[header]);

			// そうでないときは四角を表示
			else
				printf("□");

			return OUTPUT;
			break;
		case '[':
			start = *index;
			while (bf_memory->cell[header])
			{
				if (*index > 201)
				{
					printf("*index = %d\n", *index);
					puts("------");
				}
				// debug
				// if (*index == 213)
				// 	printf("cell: %d header: %d\n", bf_memory->cell[header], header);
				// if (*index == 246)
				// 	printf("cell: %d header: %d\n", bf_memory->cell[header], header);
				// if (*index >= 170)
				// {
				// 	printf("%d\n", bf_memory->cell[header]);
				// }

				loop++;

				// debug
				// if (*index >= 170)
					// fprintf(stderr, "start:cell[header]=%d\n", bf_memory->cell[header]);

				*index = code_run_loop(bf_buffer, start);

				// debug
				// if (*index >= 170)
					// fprintf(stderr, "\nend:cell[header]=%d\n", bf_memory->cell[header]);
			}

			// debug
			// if (*index >= 170)
				// puts("out of while");

			return LOOP_START;
			break;
		case ']':
			if (!loop)
			{
				return ERROR;
			}
			loop--;
			return LOOP_END;
			break;
		case '!':
			printf("[dbg]");
			return SKIP;
			break;
	}
	return SKIP;
}

int code_run_loop(BUFFER *bf_buffer, int index)
{
	while (1)
	{
		index++;
		// DBG("%d\n", BUF_LEN);
		// if (index > sizeof(bf_buffer->value))
		// {
		// 	DBG("index:%d\n", index);
		// }
		if (code_run(bf_buffer, &index) == LOOP_END)
		{
			return index;
		}
	}
}

// 表示できない制御文字を判定
bool code_check(char code)
{
	// tab、改行、esc、space、文字は表示に問題ない
	if (code == 9 || code == 10 || code == 27 || code == 32 || (code >= 33 && code <= 126))
	{
		return true;
	}
	else
	{
		return false;
	}
}

MEMORY *memory_new(MEMORY *bf_memory, BF_OPERATOR op)
{
	if (op == NEXT)
	{
		bf_memory->next = (MEMORY*) malloc(sizeof(MEMORY));
		bf_memory->next->prev = bf_memory;
		bf_memory = bf_memory->next;
	}
	else if (op == PREV)
	{
		bf_memory->prev = (MEMORY*) malloc(sizeof(MEMORY));
		bf_memory->prev->next = bf_memory;
		bf_memory = bf_memory->prev;
	}
	memory_init(bf_memory);
	return bf_memory;
}

void memory_init(MEMORY *bf_memory)
{
	int i;
	for (i=0; i<sizeof(bf_memory->cell); i++)
	{
		bf_memory->cell[i] = 0;
	}
}

char* source_load(FILE **fp, BUFFER *bf_buffer, int length, int argc, char *argv[])
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
	size = fread(bf_buffer->value, sizeof(char), length-1, *fp);
	if (size == EOF && !feof(*fp))
	{
		my_strerror(3, argv[0], argv[optind]);
		exit(EXIT_FAILURE);
	}
	bf_buffer->value[size] = '\0';
	return argv[optind];
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
		// 90~: source error
		case 91:
			fprintf(stderr, "no operator '[' corresponding to ']'.\n");
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
