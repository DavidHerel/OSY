EXTRA_CFLAGS ?=-fsanitize=address -fno-omit-frame-pointer
FILES=main.o buffers.o job_provider.o workplace_chain.o commands.o workplace_provider.o worker_provider.o worker.o
CC=gcc
CFLAGS=-pthread -Wall -g -O2

factory: $(FILES)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $(FILES)

run: factory
	./factory

%.o: %.c
	$(CC) -o $@ -c $(CFLAGS) $(EXTRA_CFLAGS) $<

clean: factory
	rm $(FILES)
	rm factory

zip: factory
	make clean
	zip hw06_osy.zip Makefile *.[ch]
