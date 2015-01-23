#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/time.h>
#include "getopt_long/my_getopt.h"

#define CELL_LEN 30
// #define BUF_LEN 256

// debug
// #define BUF_LEN 1024
#define BUF_LEN 10240

typedef unsigned char BYTE;

typedef struct memory
{
	BYTE cell[CELL_LEN];
	struct memory* next;
	struct memory* prev;
} MEMORY;

typedef struct buffer
{
	char value[BUF_LEN];
	struct buffer* next;
} BUFFER;

typedef enum {PLUS, MINUS, NEXT, PREV, INPUT, OUTPUT, LOOP_START, LOOP_END, ERROR, SKIP} BF_OPERATOR;

// 端末設定保存用大域変数
struct termios CookedTermIos; // cooked モード用
struct termios MyTermIos; // 入力時用オリジナルモード用

char* source_load(FILE **fp, BUFFER *bf_buffer, int length, int argc, char *argv[]);
void my_strerror(short errcode, const char *com, const char *option);
void usage(const char *com);
void checkopt(int argc, char *argv[]);
BF_OPERATOR code_run(BUFFER *bf_buffer, int *index);
void memory_init(MEMORY *bf_memory);
MEMORY *memory_new(MEMORY *bf_memory, BF_OPERATOR op);
bool code_check(char code);
int code_run_loop(BUFFER *bf_buffer, int index);
void my_termios_init(void);
void exit_signal(char sig);
int my_getchar(int interval);
