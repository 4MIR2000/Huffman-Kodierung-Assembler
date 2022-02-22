CC=gcc
CFLAGS=-O3 -g -Wall -Wextra

.PHONY: all
all: main
main: Rahmenprogramm/frame.c Häufigkeitsanalyse/frequency_analysis.S Häufigkeitsanalyse/quicksort_result.S Wörterbuch/dictionary.S Kodierung/coding.S Rahmenprogramm/huffman_encode.S Tests/test.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f main *.bin *~ *.txt
