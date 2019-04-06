CC=gcc
CFLAGS=-g -Wall -Iinclude

%.o: %.c
	$(CC) -o $@ -c $(CFLAGS) $<

bin: nsd forkpipe

forkpipe: forkpipe.o
	$(CC) -o forkpipe forkpipe.c $(CFLAGS)

nsd: nd.o nsd.o nsd_main.o nd.h nsd.h
	$(CC) -o nsd nd.o nsd.o nsd_main.o $(CFLAGS)

zip:
	tar czf osy04.tgz Makefile *.[ch]
