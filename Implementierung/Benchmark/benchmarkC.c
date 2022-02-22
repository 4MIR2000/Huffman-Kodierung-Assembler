#include <stdio.h>
#include <time.h>
#include "../Vergleichsimplementierungen/huffmanC.c"

void test(FILE *filePath, FILE *schreiben, int groesse, int wiederholungen,
	  int testNummer) {

	struct timespec start, ende;
	double zeit, durchschnitt;

	char buffer[groesse];

	fgets(buffer, groesse, (FILE *)filePath);

	int neueGroesse = (groesse * 4) + 1;

	char result[neueGroesse];

	printf("-----------TEST %d-----------\n\nGröße von data: %d bytes\n\n",
	       testNummer, groesse);

	clock_gettime(CLOCK_MONOTONIC, &start);

	for (int i = 0; i < wiederholungen; i++) {
		huffman_encode(buffer, result, neueGroesse);
	}

	clock_gettime(CLOCK_MONOTONIC, &ende);

	zeit =
	    ende.tv_sec - start.tv_sec + 1e-9 * (ende.tv_nsec - start.tv_nsec);

	durchschnitt = zeit / wiederholungen;

	printf("Benötigte Zeit: %.f \xC2\xB5s\n\n", durchschnitt * 1e+6);

	fprintf((FILE *)schreiben, "%.f %s", durchschnitt * 1e+6,
		" \xC2\xB5s\n");

	fseek(filePath, 0, SEEK_SET);
}

int main(int argc, char *argv[]) {

	if (argc == 1) {
		printf("./name [NAME DER OUTPUTDATEI]\n\n WICHTIG: Es muss "
		       "eine huffman_encode Funktion im Ordner existieren\n");
		return -1;
	}

	int wiederholungen = 20;
	FILE *filePath;
	filePath = fopen("./Benchmark/sample.txt", "r");

	FILE *schreiben = fopen(argv[1], "w");

	test(filePath, schreiben, 1000, wiederholungen, 0);

	for (int i = 1; i <= 5; i++) {
		test(filePath, schreiben, i * 20000, wiederholungen, i);
	}

	fclose(filePath);

	fclose(schreiben);
}
