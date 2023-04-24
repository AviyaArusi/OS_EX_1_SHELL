CC = gcc
CFLAGS = -Wall -Wextra -Werror

.PHONY: all clean

all: encode decode

encode: encode.o codecA.o codecB.o
	$(CC) $(CFLAGS) -o $@ $^

decode: decode.o codecA.o codecB.o
	$(CC) $(CFLAGS) -o $@ $^

encode.o: encode.c codecA.h codecB.h
	$(CC) $(CFLAGS) -c $<

decode.o: decode.c codecA.h codecB.h
	$(CC) $(CFLAGS) -c $<

codecA.o: codecA.c codecA.h
	$(CC) $(CFLAGS) -c $<

codecB.o: codecB.c codecB.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o encode decode
