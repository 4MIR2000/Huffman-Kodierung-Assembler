#include "../Benchmark/runBenchmark.c"
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct dataStruct *readFile(char *filePath);
void writeFile(char *filePath, char *result, int size);

struct dataStruct {
	unsigned int size;
	char *data;
};

extern int huffman_encode(char *data, char *result, unsigned int result_size);
extern void test();


void binar(unsigned char n) {
	unsigned i;
	for (i = 128; i > 0; i = i / 2) {
		if (n & i)
			printf("1");
		else
			printf("0");
	}
}

int main(int argc, char *argv[]) {
	char *inputPath = NULL;
	char *outputPath = NULL;
	struct dataStruct *dataAndSize = NULL;

	char opt;
	bool containsInputPath;
	bool containsOutputPath;

	if (argc == 1) {
		printf("Benutzen sie das Argument -h um zu sehen wie man das "
		       "Programm verwendet.\n");
		exit(EXIT_FAILURE);
	}

	while ((opt = getopt(argc, argv, "bthi:o:")) != -1) {
		switch (opt) {
		case 'i':
			inputPath = optarg;
			containsInputPath = true;
			dataAndSize = readFile(inputPath);
			break;
		case 'o':
			outputPath = optarg;
			containsOutputPath = true;
			break;
		case 'h':
			printf(
			    "Verwendung: ./main [-i \"Dateipfad\"] [-s "
			    "Größe]...\n"
			    "Kodiert die Input Datei in der "
			    "Huffman-Kodierung und gibt diese "
			    "auf der Konsole aus sowie speichert die "
			    "Ausgabe in eine Datei (empfohlen wird eine Binary "
			    "Datei .bin zu nehmen)\n\n"
			    "Argumente:\n"
			    "\t -i \"Dateipfad\"\t\twähle die Datei aus "
			    "welche kodiert werden soll\n"
			    "\t -o \"Dateipfad\"\t\twähle den Namen der "
			    "Output Datei aus. Falls diese schon existiert "
			    "wird die Datei überschrieben, sonst wird eine "
			    "neue Datei erstellt\n"
			    "\t -t \t\t\tFührt automatische Tests aus zur "
			    "Überprüfung der Korrektheit der huffman_encode "
			    "Methode (Prüft korrekte und falsche Eingaben)\n"
			    "\t -b \t\t\tFührt den Benchmark aus. "
			    "Zeitmessungen kommen in den Ordner Benchmark. "
			    "Bitte nicht den File sample.txt löschen!\n"
			    "\t -h \t\t\tzeige diese Hilfe and und beende das "
			    "Programm\n\n"
			    "AUSGABE: Auf der Konsole werden nach der "
			    "Ausführung die Bytes, welche in die Datei "
			    "geschrieben werden, auf der Konsole ausgegeben.\n"
			    "Der Grund hierfür ist der, dass gewöhnliche "
			    "Programme in bestimmten Fällen die Datei nicht "
			    "korrekt lesen können.\n(Falls ein Zeichen mit "
			    "einer 0 kodiert wird und den Byte vollständig "
			    "füllt)\nAls Erstes wird das Wörterbuch "
			    "ausgegeben, dann ein Splitter Byte mit der 1 und "
			    "danach die Kodierung.\nAbhängig davon ob die "
			    "Kodierung mit einer 1 oder einer 0 endet und ob "
			    "im Byte freien Platz ist, wird dieser mit dem "
			    "gegesätzlichen Bit des letzen Codeworts "
			    "aufüllt.\nDas zweite Bit beim Splitter wird dann "
			    "auf 1 oder 0 gesetzt falls das letzte Codewort "
			    "mit einer 1 oder 0 endet.\n");
			exit(EXIT_FAILURE);
			break;
		case 't':
			printf("Führe Tests aus...\n\n");
			test();
			exit(EXIT_SUCCESS);
			break;
		case 'b':
			bench();
			exit(EXIT_SUCCESS);
			break;
		default:
			fprintf(stderr,
				"Verwendung: %s [-i \"filepath\"] [-s size]\n",
				argv[0]);
			exit(EXIT_FAILURE);
			break;
		}
	}

	if (!containsInputPath) {
		printf("Es wurde keine input-Datei angegeben.\n");
		exit(EXIT_FAILURE);
	}

	if (!containsOutputPath) {
		printf("Es wurde keine Output-Datei angeben.\n");
		exit(EXIT_FAILURE);
	}

	unsigned int sizeOfResult = (dataAndSize->size) * 8;

	char result[sizeOfResult];
	int return_value =
	    huffman_encode(dataAndSize->data, result, sizeOfResult);

	// Speicher freigeben
	if (dataAndSize != NULL) {
		if (dataAndSize->data != NULL) {
			free(dataAndSize->data);
		}
		free(dataAndSize);
	}

	if (return_value <= 0) {
		printf("Fehlerhafter Programmablauf. Gründe hierfür können "
		       "sein: Das Eingabewort hat die Länge 0; es soll ein "
		       "Zeichen verarbeitet werden, welches nicht von "
		       "7-Bit-ASCII kodiert wird; ein Zeichen kommt mehr als "
		       "65535 mal vor; es kommen insgesamt zu viele Zeichen "
		       "vor, um den Huffmanbaum zu erstellen.\n");
		exit(EXIT_FAILURE);
	}

	writeFile(outputPath, result, return_value);
	exit(EXIT_SUCCESS);
}

void writeFile(char *filePath, char *result, int size) {
	FILE *file = fopen(filePath, "w");
	if (file == NULL) {
		printf("Output-Datei konnte nicht geöffnet werden\n");
		exit(EXIT_FAILURE);
	}

	int i = 0, j = 0;
	while (i < size) {
		if (j == 10) {
			printf("\n");
			j = 0;
		}
		binar(result[i]);
		printf(" ");
		i++;
		j++;
	}
	printf("\n");

	fprintf(file, "%.*s", size, result);

	// File wieder schließen
	fclose(file);
}

struct dataStruct *readFile(char *filePath) {
	FILE *file = fopen(filePath, "r");
	if (file == NULL) {
		printf("Input Datei konnte nicht geöffnet werden.\n");
		exit(EXIT_FAILURE);
	}

	struct dataStruct *textAndSize = malloc(sizeof(struct dataStruct));
	if (textAndSize == NULL) {
		printf("Speicher Allokation Fehler!");
		exit(EXIT_FAILURE);
	}

	int c;
	size_t index = 0;

	if (file == NULL) {
		fprintf(stderr, "Konnte Datei in %s nicht öffnen!\n", filePath);
		exit(EXIT_FAILURE);
	}

	// Mache den Cursor auf das Ende des Files. ftell sagt uns dann wo
	// dieser ist
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	textAndSize->data = malloc(file_size);

	// wichtig: Malloc überprüfung check
	if (textAndSize->data == NULL) {
		fprintf(stderr, "Memory Allokation Fehler!");
		exit(EXIT_FAILURE);
	}

	while ((c = fgetc(file)) != EOF) {
		textAndSize->data[index++] = (char)c;
	}

	textAndSize->data[index - 1] = '\0';
	textAndSize->size = index;

	// Datei schließen und Speicher freigeben
	fclose(file);
	return textAndSize;
}
