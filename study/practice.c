#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LEN 1024
#define MEM_LEN 30000

typedef unsigned char BYTE;

FILE *sourceLoad(int argc, char *argv[], char *buffer);
void loadMore(FILE *fp, char *_buffer);
void codeRun(char code);
char checkCode(char code);

int main(int argc, char *argv[])
{
	int i;
	FILE *fp;
	char buffer[BUF_LEN];

	// ファイルの読み込み
	fp = sourceLoad(argc, argv, buffer);

	while (1)
	{
		for (i=0; buffer[i] != '\0'; i++)
		{
			// ソースファイルを一文字ずつ表示できることを確認
			// printf("buffer[%d]:%c\n", i, buffer[i]);
			codeRun(buffer[i]);
		}

		// bufferがいっぱいまで入っていたら追加で読み込み
		if (i == BUF_LEN-1)
		{
			loadMore(fp, buffer); // ファイルを追加で読み込み
		}
		// いっぱいまで使っていなかったらファイルの見込みを終了
		else
		{
			fclose(fp);
			break;
		}
	}

	// puts("");
	return 0;
}

// 追加でファイルの読み込み
void loadMore(FILE *fp, char *_buffer)
{
	int size;
	size = fread(_buffer, sizeof(char), BUF_LEN-1, fp);
	_buffer[size] = '\0';
}

// ファイルの読み込み（初回時）
FILE *sourceLoad(int argc, char *argv[], char *_buffer)
{
	char *fname;	// ソースファイルの名前
	FILE *fp;
	// long length; // ファイルの長さを格納
	int size;		// freadの戻り値を格納

	// インライン引数がないとき、多いときはエラーとUsageを表示
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

	// インライン引数で-h,--helpのときはUsageを表示
	if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
	{
		puts("Usage: ./bf <source>");
		exit(EXIT_SUCCESS);
	}

	fname = argv[1];
	fp = fopen(fname, "r"); // 読み込み用にファイルを開く
	// ファイルが開けなかったらエラーを表示して終了
	if(fp == NULL)
	{
		puts("file not found.");
		exit(EXIT_FAILURE);
	}

	/* ファイルの長さを計測（無効化）
	 * ※あまりいいメソッドではなさそう
	 * ※そもそも必要ないかも */
	// fseek(fp, 0L, SEEK_END); // ファイルポインタをファイル終点にする
	// length = ftell(fp); // ファイルポインタの位置を取得し、ファイルの長さを測定
	// fseek(fp, 0L, SEEK_SET); // ファイルポインタをファイル始点に戻す

	/* ファイル全体を格納するメモリを割り当てる */
	// buffer = (char*) malloc(length);
	// buffer = (char*) malloc(2);

	/* ファイルの読み込み
	 * 一回で1023Byte分読み込み、最後にNULLを挿入 */
	size = fread(_buffer, sizeof(char), BUF_LEN-1, fp);
	_buffer[size] = '\0';
	// fread()の結果確認
	// printf("size:%d\n", size);
	// printf("buffer:%s\n", _buffer);

	return fp;
}

// 引数に取った文字に応じて、処理を実行
void codeRun(char code)
{
	static BYTE memory[MEM_LEN] = {0};
	static short curmem = 0;
	char temp;
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
				break;
		case '<':
				curmem--;
				break;
		case '.':
				// 表示に問題ない文字は表示
				if (!checkCode(memory[curmem]))
				{
					putchar(memory[curmem]);
				}
				// // そうでないときはエラーを吐いて終了
				// else
				// {
				// 	puts("\n[Error]Tryed to display a but character.");
				// 	exit(EXIT_FAILURE);
				// }
				// よりもこっちのほうがいい？
				else
				{
					printf("□");
				}
				break;
		case ',':
				printf("\ninput a 1byte character: ");
				temp = getchar();
				memory[curmem] = temp;
				// 二文字目以降があるときは捨てる
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

	// メモリーがマイナスになるか用意した値を超えたら循環させる
	if(curmem < 0) curmem = MEM_LEN-1;
	else if(curmem > MEM_LEN) curmem = 0;

}

// 表示できない制御文字を判定
char checkCode(char code)
{
	// tab、改行、esc、space、文字は表示に問題ない
	if (code == 9 || code == 10 || code == 27 || code == 32 || (code >= 33 && code <= 126))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

