#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "getopt_long/my_getopt.h"

#define DBG(...) (printf("%s %u @%s(): ",__FILE__,__LINE__,__func__), printf(__VA_ARGS__)), puts("")
#define BUF_LEN 1024
#define CELL_LEN 30

typedef unsigned char BYTE;

typedef struct memory
{
	BYTE cell[CELL_LEN];
	struct memory* next;
	struct memory* prev;
} MEMORY;

typedef enum {PLUS, MINUS, NEXT, PREV, INPUT, OUTPUT, LOOP_START, LOOP_END, ERROR} bf_Operator;

char* source_load(FILE **fp, char *buffer, int length, int argc, char *argv[]);
void my_strerror(short errcode, const char *com, const char *option);
void usage(const char *com);
void checkopt(int argc, char *argv[]);
bf_Operator code_run(char *buffer, int *index);
void memory_init(MEMORY *bf_memory);
MEMORY *memory_new(MEMORY *bf_memory, bf_Operator op);
bool code_check(char code);
int code_run_loop(char *buffer, int index);
