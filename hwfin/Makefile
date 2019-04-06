CC=gcc
CFLAGS= -pthread -g -Wall -Iinclude
EXTRA_CFLAGS ?=-fsanitize=address -fno-omit-frame-pointer

%.o: %.c
	$(CC) -o $@ -c $(CFLAGS) $<

factory: factory.o
	$(CC) -o factory factory.c $(CFLAGS) $(EXTRA_CFLAGS)

