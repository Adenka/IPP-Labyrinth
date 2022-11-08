CC=gcc
CFLAGS=-g -Wall -Wextra -Wno-implicit-fallthrough -std=c17 -O2
DEPS=kolejka.o bitset.o hex.o ciag.o wejscie.o main.o pomocnicze.o

all: main

main: $(DEPS)
	$(CC) $(CFLAGS) -o labyrinth $(DEPS)

pomocnicze.o: pomocnicze.c pomocnicze.h
	$(CC) $(CFLAGS) -c pomocnicze.c

kolejka.o: kolejka.c kolejka.h pomocnicze.h
	$(CC) $(CFLAGS) -c kolejka.c

hex.o: hex.c hex.h kolejka.h pomocnicze.h
	$(CC) $(CFLAGS) -c hex.c

bitset.o: bitset.c bitset.h
	$(CC) $(CFLAGS) -c bitset.c

ciag.o: ciag.c ciag.h kolejka.h bitset.h
	$(CC) $(CFLAGS) -c ciag.c

wejscie.o: wejscie.c wejscie.h ciag.h hex.h pomocnicze.h
	$(CC) $(CLAGS) -c wejscie.c

main.o: main.c wejscie.h ciag.h hex.h
	$(CC) $(CLAGS) -c main.c

.PHONY: all clean

clean:
	rm -f *.o labirynth
