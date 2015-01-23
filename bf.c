#include "bf.h"

#define DBG(...) (printf("%s %u @%s(): ",__FILE__,__LINE__,__func__), printf(__VA_ARGS__)), puts("")

static int dev_debug = 0; // debug用コードの有効化

static bool enable_debug = false; // dオプションでdebug用文字の処理を有効に

int main(int argc, char *argv[])
{
	FILE *fp = NULL;
	BUFFER bf_buffer;
	int i;

	my_termios_init();
	check_args(argc, argv);
	fp = file_open(argv);

	source_load(&bf_buffer, sizeof(bf_buffer.value), fp, argv);

	while (1)
	{
		for (i=0; bf_buffer.value[i] != '\0'; i++)
		{
			if (code_run(&bf_buffer, &i) == ERROR) {
				puts("");
				my_strerror(91, argv[optind], NULL);
				exit(EXIT_FAILURE);
			}
		}

		// fpが終端に達していなければ追加で読み込み
		// 終端に達していたらファイルの見込みを終了
		if (source_load(&bf_buffer, sizeof(bf_buffer.value), NULL, NULL) < 0)
		{
			if (fclose(fp) == EOF) {
				my_strerror(2, argv[0], argv[optind]);
				exit(EXIT_FAILURE);
			}
			break;
		}
	}
	return EXIT_SUCCESS;
}

void check_args(int argc, char *argv[])
{
	// 引数がないとき
	if(argc == 1) {
		my_strerror(13, argv[0], NULL);
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	// オプションの処理
	checkopt(argc, argv);

	// オプション関係以外に引数がないとき
	if (argc == optind) {
		my_strerror(13, argv[0], NULL);
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	// オプション以外の引数が2つ以上のとき
	else if (argc - optind > 1) {
		my_strerror(12, argv[0], NULL);
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
}

FILE *file_open(char *argv[])
{
	FILE *fp;
	if ((fp = fopen(argv[optind], "r")) == NULL) {
		my_strerror(1, argv[0], argv[optind]);
		exit(EXIT_FAILURE);
	}
	return fp;
}

void my_wait()
{
	static bool flag = true;
	if(flag) {
		flag = false;
		printf("_\b");
		fflush(stdout);
	}
	else {
		flag = true;
		printf(" \b");
		fflush(stdout);
	}
}

int my_getchar(int interval)
{
	char c = 0;
	static struct itimerval itimer;
	static bool init_flag = true;

	if (init_flag) {
		struct sigaction sa;
		// シグナルハンドラの設定
		memset(&sa, 0, sizeof(struct sigaction));
		sa.sa_handler = my_wait;
		//sa.sa_flags   = SA_RESTART;
		if (sigaction(SIGALRM, &sa, NULL) != 0) {
			perror("sigaction");
			return 0;
		}
		// タイマーの設定
		itimer.it_value.tv_sec  = itimer.it_interval.tv_sec  = interval; // sec
		itimer.it_value.tv_usec = itimer.it_interval.tv_usec = 0; // micro sec
		init_flag = false;
	}
	if (setitimer(ITIMER_REAL, &itimer, NULL) < 0) {
		perror("setitimer");
		return 0;
	}

	c = getchar();

	return c;
}

void my_termios_init(void)
{
	// 初期状態の端末設定 (cooked モード) を取得・保存する．
	tcgetattr(STDIN_FILENO, &CookedTermIos);

	// 入力時用の端末設定を作成・保存する．
	MyTermIos = CookedTermIos;
	// cfmakeraw(&MyTermIos);
	// MyTermIos.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	// MyTermIos.c_oflag &= ~OPOST;
	// MyTermIos.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	// MyTermIos.c_cflag &= ~(CSIZE | PARENB);
	// MyTermIos.c_cflag |= CS8;

	// 入力を表示しない、エンターを待たない、特殊文字にに対応しない(ctrl+c, ctrl+\は手動で対応)
	MyTermIos.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG);
	return;
}

void exit_signal(char sig)
{
	// 端末設定を元に戻す．
	tcsetattr(STDIN_FILENO, 0, &CookedTermIos);
	if (sig == 0x03) {
		printf("^C");
		exit(130);
	}
	else if (sig == 0x1c) {
		printf("^\\");
		exit(131);
	}
	else {
		printf("bad exit signal.");
		exit(EXIT_FAILURE);
	}
}

BF_OPERATOR code_run(BUFFER *bf_buffer, int *index)
{
	static MEMORY *bf_memory;
	static short header;
	static int loop = 0;
	char input;
	int start;
	int tmp_loop;

	if (bf_memory == NULL) {
		bf_memory = (MEMORY*) malloc(sizeof(MEMORY));
		header = sizeof(bf_memory->cell)/2;
		memory_init(bf_memory);
	}
	switch (bf_buffer->value[*index])
	{
		case '+':
			bf_memory->cell[header]++;
			return PLUS;
			break;
		case '-':
			bf_memory->cell[header]--;
			return MINUS;
			break;
		case '>':
			if (header == 29) {
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
			if (header == 0) {
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
			// 端末を変更する
			tcsetattr(STDIN_FILENO, 0, &MyTermIos);
			printf("  (Press any key to input.)\x1b[27D");
			while (1)
			{
				input = my_getchar(1);
				while (input == -1)
				{
					input = my_getchar(1);
				}
				if (input == 0x03 || input == 0x1c) {
					exit_signal(input);
				}
				else {
					bf_memory->cell[header] = input;
					break;
				}
			}
			// 端末設定を元に戻す．
			tcsetattr(STDIN_FILENO, 0, &CookedTermIos);
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
			if (bf_memory->cell[header]) {
				while (bf_memory->cell[header])
				{
					loop++;
					*index = code_run_loop(bf_buffer, start);
				}
			}
			else {
				tmp_loop = loop;
				loop++;
				while (tmp_loop < loop)
				{
					*index = *index + 1;
					if (bf_buffer->value[*index] == '[') {
						loop++;
					}
					else if (bf_buffer->value[*index] == ']') {
						loop--;
					}
				}
			}
			return LOOP_START;
			break;
		case ']':
			if (!loop) {
				return ERROR;
			}
			loop--;
			return LOOP_END;
			break;
		case '!':
			if (enable_debug) {
				printf("[dbg]");
				fflush(stdout);
			}
			return SKIP;
			break;
		case '?':
			if (enable_debug) {
				printf("[dbg: memory=%d]", bf_memory->cell[header]);
				fflush(stdout);
			}
			return SKIP;
			break;
		case '#':
			if (enable_debug) {
				// 端末を変更する
				tcsetattr(STDIN_FILENO, 0, &MyTermIos);
				printf("  (Press enter to continue.)\x1b[28D");
				while (1)
				{
					input = my_getchar(1);
					while (input == -1)
					{
						input = my_getchar(1);
					}
					if (input == '\n') {
						break;
					}
					else if (input == 0x03 || input == 0x1c) {
						exit_signal(input);
					}
				}
				// 端末設定を元に戻す．
				tcsetattr(STDIN_FILENO, 0, &CookedTermIos);
			}
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

		// buffer終端まで読み込んだときに追加読み込み
		if (bf_buffer->value[index] == '\0') {
			puts("loop error.");
			exit(EXIT_FAILURE);
		}

		if (code_run(bf_buffer, &index) == LOOP_END) {
			return index;
		}
	}
}

// 表示できない制御文字を判定
bool code_check(char code)
{
	// tab、改行、space、文字は表示に問題ない
	if (code == 9 || code == 10 || code == 32 || (code >= 33 && code <= 126)) {
		return true;
	}
	// escは表示に問題ないが、表示されない
	else if (code == 27) {
		return false;
	}
	else {
		return false;
	}
}

MEMORY *memory_new(MEMORY *bf_memory, BF_OPERATOR op)
{
	if (op == NEXT) {
		bf_memory->next = (MEMORY*) malloc(sizeof(MEMORY));
		bf_memory->next->prev = bf_memory;
		bf_memory = bf_memory->next;
	}
	else if (op == PREV) {
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

int source_load(BUFFER *bf_buffer, int length, FILE *_fp, char *argv[])
{
	int size; // freadの戻り値を格納
	static FILE *fp = NULL;
	static char *com;
	static char *source;

	if (_fp != NULL) {
		fp = _fp;
	}
	if (argv != NULL) {
		com = (char*) malloc(strlen(argv[0]) + 1);
		source = (char*) malloc(strlen(argv[optind]) + 1);
		strncpy(com, argv[0], strlen(argv[0]) + 1);
		strncpy(source, argv[optind], strlen(argv[optind]) + 1);
	}

	// fpが終端に達していたら-1を返す
	if (feof(fp)) {
		return -1;
	}

	size = fread(bf_buffer->value, sizeof(char), length-1, fp);
	if (size == EOF && !feof(fp)) {
		my_strerror(3, argv[0], argv[optind]);
		exit(EXIT_FAILURE);
	}
	bf_buffer->value[size] = '\0';
	return 0;
}

void my_strerror(short errcode, const char *com, const char *option)
{
	// ~100: error
	if (errcode < 100) {
		fprintf(stderr, "%s: \x1b[31merror\x1b[39m: ", com);
	}
	// 100~: warnig
	else {
		fprintf(stderr, "%s: \x1b[33mwarning\x1b[39m: ", com);
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
		if (argv[i][0] == '-') {
			if (argv[i][1] == '-') {
				if (!strncmp(argv[i], "--help", 3)) {
					opterr = 0; // エラーメッセージを非表示にする
				}
			}
			else {
				for (j=1; j<strlen(argv[i]); j++)
				{
					if (argv[i][j] == 'h') {
						opterr = 0; // エラーメッセージを非表示にする
					}
				}
			}
		}
	}

	struct option long_options[] =
	{
		{"help", no_argument, &long_opt, 'h'},
		{0, 0, 0, 0}
	};

	while((opt = my_getopt_long(argc, argv, "hd", long_options, &option_index)) != -1)
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
			case 'd':
				enable_debug = true;
				break;
			case '?':
				// Unknown or required argument.
				break;
		}
	}
	return;
}
