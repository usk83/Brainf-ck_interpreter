# Makefile

CC = gcc
CFLAGS = -Wall -o $@

target = bf
src = prototype.c getopt_long/my_getopt_long.c
header = prototype.h getopt_long/my_getopt.h

.PHONY: all
all: $(target)

$(target): $(src) $(header)
	$(CC) $(CFLAGS) $(src)

.PHONY: clean
clean:
	rm -rf $(target)
