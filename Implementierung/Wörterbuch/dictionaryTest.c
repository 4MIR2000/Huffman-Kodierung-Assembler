#include <stdio.h>

extern unsigned int dictionary(unsigned short *, unsigned int size);

void binar(unsigned char n) {
	unsigned i;
	for (i = 128; i > 0; i = i / 2) {
		if (n & i)
			printf("1");
		else
			printf("0");
	}
}

int main(int argc, const char *argv[]) {

	printf("--- TEST 1 ---\n");

	unsigned short tabelle[] = {'A', 1727, 'B', 3, 'R', 2, 'K', 1,
				    'D', 1, 0,	 0, 0,	 0, 0,	 0};

	for (int i = 0; tabelle[i] != 0; i += 2) {
		if (tabelle[i] != 0) {
			printf("Char %c: %i\n", tabelle[i],
			       tabelle[i + 1]);
		}
	}

	unsigned int dictSize = dictionary(tabelle, 20);
	printf("Größe des Wörterbuchs: %u\n", dictSize);

	for (int i = 0; i < dictSize / 2; i++) {
		unsigned short tmp = tabelle[i];
		printf("Buchstabe: %c, Kodierung: ", tmp & 127);
		binar(tmp >> 8);
		printf("\n");
	}

	printf("Sollte 1 sein: %u\n\n", tabelle[dictSize / 2]);

	printf("--- TEST 2 ---\n");

	unsigned short tabelle2[] = {'j', 10, 'g', 7, 'U', 7, '2', 5,
				     'b', 3,  0,   0, 0,   0, 0,   0};

	for (int i = 0; tabelle2[i] != 0; i += 2) {
		if (tabelle2[i] != 0) {
			printf("Char %c: %i\n", tabelle2[i] & 255,
			       tabelle2[i + 1] & 255);
		}
	}

	dictSize = dictionary(tabelle2, 20);
	printf("Größe des Wörterbuchs: %u\n", dictSize);

	for (int i = 0; i < dictSize / 2; i++) {
		unsigned short tmp = tabelle2[i];
		printf("Buchstabe: %c, Kodierung: ", tmp & 127);
		binar(tmp >> 8);
		printf("\n");
	}

	printf("Sollte 1 sein: %u\n\n", tabelle2[dictSize / 2]);

	printf("--- TEST 3 ---\n");

	unsigned short tabelle3[] = {20,  4, 'a', 2, 's', 2, 'i', 2, 't', 2,
				     'e', 2, 'g', 2, 'D', 1, 'n', 1, 'u', 1,
				     'r', 1, 'T', 1, '!', 1, 0,	  0};

	for (int i = 0; tabelle3[i] != 0; i += 2) {
		if (tabelle3[i] != 0) {
			printf("Char %c: %i\n", tabelle3[i] & 255,
			       tabelle3[i + 1] & 255);
		}
	}

	dictSize = dictionary(tabelle3, 52);
	printf("Größe des Wörterbuchs: %i\n", dictSize);

	for (int i = 0; i < dictSize / 2; i++) {
		unsigned short tmp = tabelle3[i];
		printf("Buchstabe: %c, Kodierung: ", tmp & 127);
		binar(tmp >> 8);
		printf("\n");
	}

	printf("Sollte 1 sein: %u\n", tabelle3[dictSize / 2]);
}
