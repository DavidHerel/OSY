CC=gcc
CFLAGS= -nostdlib -nostdinc -static -m32 -Wall -g -O2

%.o: %.c
	$(CC) -o $@ -c $(CFLAGS) $<

bin: hexconv

hexconv: hexconv.o
	$(CC) -o hexconv hexconv.c $(CFLAGS)

