CC=gcc
CFLAGS=-O3 -g -Wall -Wextra

.PHONY: all
all: test
test: codingTest.c coding.S
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f main *~
