CC=gcc
CFLAGS=-O0

all: main

main: dictionaryTest.c dictionary.S
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f main
