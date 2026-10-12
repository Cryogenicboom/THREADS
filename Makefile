CFLAGS = -g -Wall -Werror -Isrc/include
CC = gcc

FILES = src/xthreads.c \
src/include/xthreads.h \
Testcase.c

OUT = thread

all:
	$(CC) $(CFLAGS) $(FILES) -o $(OUT)

run: 
	./$(OUT)

clean:
	rm -f $(OUT)