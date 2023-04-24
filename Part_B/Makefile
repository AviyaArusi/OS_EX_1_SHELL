CC = gcc
CFLAGS = -Wall -fPIC

all: libcodecA.so libcodecB.so encode decode

libcodecA.so: codecA.o
	$(CC) -shared -o $@ $^

libcodecB.so: codecB.o
	$(CC) -shared -o $@ $^

codecA.o: codecA.c codec.h
	$(CC) $(CFLAGS) -c $<

codecB.o: codecB.c codec.h
	$(CC) $(CFLAGS) -c $<

encode: encode.c codec.h
	$(CC) $(CFLAGS) -o $@ $< -ldl

decode: decode.c codec.h
	$(CC) $(CFLAGS) -o $@ $< -ldl

clean:
	rm -f *.o *.so encode decode
