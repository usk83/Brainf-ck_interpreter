#define DBG(...) (printf("%s %u @%s(): ",__FILE__,__LINE__,__func__), printf(__VA_ARGS__)), puts("")
#define BUF_LEN 1024

FILE *sourceLoad(FILE *fp, char *buffer, int length, int argc, char *argv[]);
void my_strerror(short errcode, const char *com, const char *option);
void usage(const char *com);
void checkopt(int argc, char *argv[]);