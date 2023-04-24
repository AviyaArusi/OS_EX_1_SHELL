CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: cmp copy

cmp: cmp.c
	$(CC) $(CFLAGS) cmp.c -o cmp

copy: copy.c
	$(CC) $(CFLAGS) copy.c -o copy

clean:
	rm -f cmp copy
