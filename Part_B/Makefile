CC = gcc
CFLAGS = -Wall -Wextra -Werror
LIBS = -L. -lcodecA -lcodecB

.PHONY: all clean

all: libcodecA.so libcodecB.so encode decode

encode: encode.o codecA.o codecB.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

decode: decode.o codecA.o codecB.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

encode.o: encode.c codecA.h codecB.h
	$(CC) $(CFLAGS) -c $<

decode.o: decode.c codecA.h codecB.h
	$(CC) $(CFLAGS) -c $<

codecA.o: codecA.c codecA.h
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

codecB.o: codecB.c codecB.h
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

libcodecA.so: codecA.o
	$(CC) $(CFLAGS) -shared -o $@ $^

libcodecB.so: codecB.o
	$(CC) $(CFLAGS) -shared -o $@ $^

clean:
	rm -f *.o encode decode *.so
