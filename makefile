CC=gcc
CFLAGS=-O3
DEPS = settings.h abstract.h gen.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

bf: bf.o settings.o abstract.o gen.o
	$(CC) -o bf bf.o settings.o abstract.o gen.o

clean:
	rm -f *.o 
